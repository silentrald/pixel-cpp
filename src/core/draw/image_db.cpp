/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-29
 *==========================*/

#include "core/draw/image_db.hpp"
#include "core/logger/logger.hpp"
#include <cassert>

namespace draw {

inline const usize ID_SIZE = sizeof(usize);
inline const usize NEXT_ELEM_SIZE = sizeof(usize);
inline const usize PREV_ELEM_SIZE = sizeof(usize);
inline const usize LAYER_HEADER_SIZE =
    ID_SIZE + NEXT_ELEM_SIZE + PREV_ELEM_SIZE;

inline const usize ID_SENTINEL = 0U;
inline const usize INDEX_SENTINEL = UINT32_MAX;

ImageDb::ImageDb() noexcept
    : insert_index(INDEX_SENTINEL),
      head_index(INDEX_SENTINEL),
      tail_index(INDEX_SENTINEL) {}

ImageDb::ImageDb(ImageDb&& rhs) noexcept
    : ptr(rhs.ptr),
      insert_index(rhs.insert_index),
      head_index(rhs.head_index),
      tail_index(rhs.tail_index),
      capacity(rhs.capacity),
      size(rhs.size),
      bytes(rhs.bytes),
      alloc_size(rhs.alloc_size),
      disk(std::move(rhs.disk)),
      disk_size(rhs.disk_size),
      disk_capacity(rhs.disk_capacity) {
  rhs.ptr = nullptr;
}

ImageDb& ImageDb::operator=(ImageDb&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->ptr = rhs.ptr;
  rhs.ptr = nullptr;

  this->insert_index = rhs.insert_index;
  this->head_index = rhs.head_index;
  this->tail_index = rhs.tail_index;
  this->capacity = rhs.capacity;
  this->size = rhs.size;
  this->bytes = rhs.bytes;
  this->alloc_size = rhs.alloc_size;

  this->disk = std::move(rhs.disk);
  this->disk_size = rhs.disk_size;
  this->disk_capacity = rhs.disk_capacity;

  return *this;
}

void ImageDb::init(usize bytes, usize capacity) noexcept {
  this->bytes = bytes;
  this->capacity = capacity;
  this->allocate(capacity * (bytes + LAYER_HEADER_SIZE));

  this->disk.open_file(".image_db", "w+");
  this->create_image();
}

// === Copy Functions === //

void ImageDb::copy(const ImageDb& other) noexcept {
  if (this == &other) {
    return;
  }

  if (this->ptr == nullptr) {
    this->copy_empty(other);
  } else if (this->alloc_size == other.alloc_size) {
    this->copy_normal(other);
  } else if (this->alloc_size > other.alloc_size) {
    this->copy_overfit(other);
  } else {
    this->copy_grow(other);
  }
}

void ImageDb::copy_empty(const ImageDb& other) noexcept {
  // NOLINTNEXTLINE
  this->ptr = (data_ptr)std::malloc(other.alloc_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }

  this->copy_normal(other);
}

void ImageDb::copy_normal(const ImageDb& other) noexcept {
  std::memcpy(this->ptr, other.ptr, other.alloc_size);

  this->insert_index = other.insert_index;
  this->head_index = other.head_index;
  this->tail_index = other.tail_index;
  this->capacity = other.capacity;
  this->size = other.size;
  this->bytes = other.bytes;
  this->alloc_size = other.alloc_size;
}

void ImageDb::copy_overfit(const ImageDb& other) noexcept {
  this->size = other.size;
  this->bytes = other.bytes;

  // Recompute the new capacity value
  this->capacity = this->alloc_size / (this->bytes + LAYER_HEADER_SIZE);
  std::memset(this->ptr, 0, this->alloc_size);

  // Set the values of the next id
  for (usize i = this->size; i < this->capacity; ++i) {
    this->get_index_ptr()[i].next = i + 1U;
  }
  this->get_index_ptr()[this->capacity - 1U].next = ID_SENTINEL;
  this->insert_index = this->size;
  this->head_index = 0U;
  this->tail_index = this->capacity - 1U;

  // Manual copy
  auto* src_cursor = other.get_pixels_ptr();
  auto* dst_cursor = this->get_pixels_ptr();

  for (usize i = 0U, count = 0U; count < this->size;
       ++i, src_cursor += other.bytes) {
    // NOTE: Way to check for inf loops
    assert(src_cursor - other.ptr <= other.alloc_size);

    if (other.get_id_ptr()[i] == ID_SENTINEL) {
      continue;
    }

    this->get_id_ptr()[count] = other.get_id_ptr()[i];
    std::memcpy(dst_cursor, src_cursor, other.bytes);

    dst_cursor += this->bytes;
    ++count;
  }
}

void ImageDb::copy_grow(const ImageDb& other) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr = (data_ptr)std::realloc(this->ptr, other.alloc_size);
  if (new_ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }
  this->ptr = new_ptr;

  this->copy_normal(other);
}

void ImageDb::minicopy(const ImageDb& other) noexcept {
  if (this == &other) {
    return;
  }

  // TODO: Reuse memory if already allocated
  if (this->ptr) {
    std::free(this->ptr); // NOLINT
  }

  // Create the minified copy of the layer arena class
  this->insert_index = INDEX_SENTINEL;
  this->head_index = 0U;
  this->tail_index = other.size - 1U;
  this->size = this->capacity = other.size;
  this->bytes = other.bytes;
  this->alloc_size = other.size * (other.bytes + LAYER_HEADER_SIZE);

  // NOLINTNEXTLINE
  this->ptr = (data_ptr)std::malloc(this->alloc_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }

  // Manual Copy
  auto* src_cursor = other.get_pixels_ptr();
  auto* dst_cursor = this->get_pixels_ptr();

  for (i32 i = 0, count = 0; count < this->size;
       ++i, src_cursor += other.bytes) {
    // NOTE: Way to check for inf loops
    assert(src_cursor - other.ptr <= other.alloc_size);

    if (other.get_id_ptr()[i] == 0U) {
      continue;
    }

    this->get_id_ptr()[count] = other.get_id_ptr()[i];
    std::memcpy(dst_cursor, src_cursor, other.bytes);

    dst_cursor += this->bytes;
    ++count;
  }
}

ImageDb::~ImageDb() noexcept {
  if (this->ptr) {
    std::free(this->ptr); // NOLINT
    this->ptr = nullptr;
  }
}

// === Getters / Accessors === //

data_ptr ImageDb::get_ptr() const noexcept {
  return this->ptr;
}

data_ptr ImageDb::get_pixels(usize id) noexcept {
  assert(this->ptr != nullptr);
  assert(id <= this->disk_capacity);

  if (id == ID_SENTINEL) {
    return nullptr;
  }

  for (usize i = 0; i < this->capacity; ++i) {
    if (this->get_id_ptr()[i] != id) {
      continue;
    }

    // NOLINTNEXTLINE
    return this->get_pixels_ptr() + i * this->bytes;
  }

  // TODO: Helper function

  // Replace the head with the data stored in the disk
  auto index = this->head_index;
  this->get_id_ptr()[index] = id;

  // Update the indices
  auto* index_ptr = this->get_index_ptr();
  index_ptr[index_ptr[index].prev].next = this->tail_index;
  index_ptr[index_ptr[index].next].prev = INDEX_SENTINEL;
  index_ptr[index].prev = this->tail_index;
  this->head_index = index_ptr[index].next;
  index_ptr[index].next = INDEX_SENTINEL;
  this->tail_index = index;

  // Write memory from disk and return the data
  // NOLINTNEXTLINE
  auto* pixels_ptr = this->get_pixels_ptr() + index * this->bytes;
  this->disk.seek((id - 1U) * (4U + this->bytes) + 4U);
  this->disk.read(pixels_ptr, this->bytes);

  return pixels_ptr;
}

data_ptr ImageDb::get_pixels_fast(usize id) const noexcept {
  assert(this->ptr != nullptr);
  assert(id <= this->disk_capacity);

  if (id == ID_SENTINEL) {
    return nullptr;
  }

  for (usize i = 0; i < this->capacity; ++i) {
    if (this->get_id_ptr()[i] != id) {
      continue;
    }

    // NOLINTNEXTLINE
    return this->get_pixels_ptr() + i * this->bytes;
  }

  std::abort();
}

void ImageDb::get_pixels_slow(usize id, data_ptr pixels) const noexcept {
  assert(id != 0U && id <= this->disk_capacity);
  this->disk.seek((id - 1U) * (4U + this->bytes) + 4U);
  this->disk.read(pixels, this->bytes);
}

usize ImageDb::get_size() const noexcept {
  return this->size;
}

usize ImageDb::get_capacity() const noexcept {
  return this->capacity;
}

usize ImageDb::get_bytes_size() const noexcept {
  return this->bytes;
}

usize ImageDb::get_disk_size() const noexcept {
  return this->disk_size;
}

usize ImageDb::get_disk_capacity() const noexcept {
  return this->disk_capacity;
}

ImageDbIter ImageDb::get_iter() const noexcept {
  return {this->ptr, this->size, this->capacity, this->bytes};
}

// === Modifiers === //

usize ImageDb::create_image() noexcept {
  assert(this->ptr != nullptr);
  assert(this->size <= this->capacity);

  if (this->size == this->capacity) {
    // Replace the item at the head
    --this->size;
    this->insert_index = this->head_index;
  }

  assert(this->insert_index != INDEX_SENTINEL);
  assert(this->disk_size <= this->disk_capacity);

  usize id = this->next_id;
  this->disk.seek((id - 1U) * (4U + this->bytes));
  if (this->disk_size == this->disk_capacity) {
    ++this->next_id;
    ++this->disk_capacity;
  } else {
    this->next_id = this->disk.read_u32();
    this->disk.move(-4);
  }

  // Memory write
  this->get_id_ptr()[this->insert_index] = id;
  auto new_index = this->get_index_ptr()[this->insert_index].next;
  this->get_index_ptr()[this->insert_index] = {
      .next = INDEX_SENTINEL, .prev = this->tail_index};
  this->insert_index = new_index;

  // Disk Write, already moved the disk pointer
  this->disk.write_u32(INDEX_SENTINEL);

  // Set the data before writing it into disk
  // NOLINTNEXTLINE
  auto* pixels = this->get_pixels_ptr() + (id - 1U) * this->bytes;
  std::memset(pixels, 0U, this->bytes);
  this->disk.write(pixels, this->bytes);
  this->disk.flush();

  // Update both sizes
  ++this->size;
  ++this->disk_size;

  this->tail_index = id;
  if (this->head_index == INDEX_SENTINEL) {
    this->head_index = id;
  }

  return id;
}

void ImageDb::write_pixels_to_disk(usize id) const noexcept {
  auto* pixels = this->get_pixels_fast(id);
  this->disk.seek((id - 1U) * (4U + this->bytes));
  this->disk.write_u32(ID_SENTINEL);
  this->disk.write(pixels, this->bytes);
  this->disk.flush();
}

// === Memory === //

void ImageDb::allocate(usize alloc_size) noexcept {
  // NOLINTNEXTLINE
  this->ptr = (data_ptr)std::malloc(alloc_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }
  this->alloc_size = alloc_size;

  // Set all ids to 0
  std::memset(this->ptr, 0U, this->capacity * sizeof(usize));

  // Set the values of the next id
  for (usize i = 0U; i < this->capacity; ++i) {
    this->get_index_ptr()[i].next = i + 1U;
  }
  this->get_index_ptr()[this->capacity - 1U].next = INDEX_SENTINEL;
  this->insert_index = 0U;
  this->head_index = INDEX_SENTINEL;
  this->tail_index = INDEX_SENTINEL;
}

// === Private Accessors === //

usize* ImageDb::get_id_ptr() const noexcept {
  return (usize*)this->ptr;
}

ImageDb::Indexing* ImageDb::get_index_ptr() const noexcept {
  // NOLINTNEXTLINE
  return (Indexing*)(this->ptr + this->capacity * ID_SIZE);
}

data_ptr ImageDb::get_pixels_ptr() const noexcept {
  // NOLINTNEXTLINE
  return this->ptr + this->capacity * LAYER_HEADER_SIZE;
}

// === Iterators === //

ImageDbIter::ImageDbIter(
    data_ptr ptr, usize size, usize capacity, usize bytes
) noexcept
    : id_ptr((usize*)ptr),
      image_ptr(ptr + capacity * LAYER_HEADER_SIZE), // NOLINT
      size(size),
      bytes(bytes) {
  while (*this->id_ptr == ID_SENTINEL) {
    ++this->id_ptr;
    this->image_ptr += this->bytes;
  }
}

ImageDbIter& ImageDbIter::operator++() {
  assert(this->id_ptr != nullptr);

  --this->size;
  if (this->size == 0U) {
    this->id_ptr = nullptr;
    return *this;
  }

  ++this->id_ptr;
  this->image_ptr += this->bytes;
  while (*this->id_ptr == ID_SENTINEL) {
    ++this->id_ptr;
    this->image_ptr += this->bytes;
  }

  return *this;
}

usize ImageDbIter::get_id() const noexcept {
  return *this->id_ptr;
}

data_ptr ImageDbIter::get_data() const noexcept {
  return this->image_ptr;
}

ImageDbIter::operator bool() const noexcept {
  return this->id_ptr != nullptr;
}

} // namespace draw

