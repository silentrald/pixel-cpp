/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-29
 *==========================*/

#include "./anim.hpp"
#include "core/logger/logger.hpp"
#include <cstdlib>
#include <cstring>

namespace draw {

inline const i32 ID_SIZE = sizeof(u32);
inline const i32 NEXT_ELEM_SIZE = sizeof(i32);
inline const i32 LAYER_HEADER_SIZE = ID_SIZE + NEXT_ELEM_SIZE;

Anim::LayerArena::LayerArena(LayerArena&& rhs) noexcept
    : ptr(rhs.ptr),
      insert(rhs.insert),
      last(rhs.last),
      capacity(rhs.capacity),
      size(rhs.size),
      bytes(rhs.bytes),
      allocated_size(rhs.allocated_size) {
  rhs.ptr = nullptr;
}

Anim::LayerArena& Anim::LayerArena::operator=(LayerArena&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->ptr = rhs.ptr;
  rhs.ptr = nullptr;

  this->insert = rhs.insert;
  this->last = rhs.last;
  this->capacity = rhs.capacity;
  this->size = rhs.size;
  this->bytes = rhs.bytes;
  this->allocated_size = rhs.allocated_size;

  return *this;
}

void Anim::LayerArena::init(i32 bytes, i32 capacity) noexcept {
  this->bytes = bytes;
  this->capacity = capacity;
  this->allocate(capacity * (bytes + LAYER_HEADER_SIZE));
}

// === Copy Functions === //

void Anim::LayerArena::copy(const LayerArena& other) noexcept {
  if (this == &other) {
    return;
  }

  if (this->ptr == nullptr) {
    this->copy_empty(other);
  } else if (this->allocated_size == other.allocated_size) {
    this->copy_normal(other);
  } else if (this->allocated_size > other.allocated_size) {
    this->copy_overfit(other);
  } else {
    this->copy_grow(other);
  }
}

void Anim::LayerArena::copy_empty(const LayerArena& other) noexcept {
  // NOLINTNEXTLINE
  this->ptr = (data_ptr)std::malloc(other.allocated_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }

  this->copy_normal(other);
}

void Anim::LayerArena::copy_normal(const LayerArena& other) noexcept {
  std::memcpy(this->ptr, other.ptr, other.allocated_size);

  this->insert = other.insert;
  this->last = other.last;
  this->capacity = other.capacity;
  this->size = other.size;
  this->bytes = other.bytes;
  this->allocated_size = other.allocated_size;
}

void Anim::LayerArena::copy_overfit(const LayerArena& other) noexcept {
  this->size = other.size;
  this->bytes = other.bytes;

  // Recompute the new capacity value
  this->capacity = this->allocated_size / (this->bytes + LAYER_HEADER_SIZE);
  std::memset(this->ptr, 0, this->allocated_size);

  // Set the values of the next id
  for (i32 i = this->size; i < this->capacity; ++i) {
    ((i32*)this->ptr)[i + this->capacity] = i + 1;
  }
  ((i32*)this->ptr)[this->capacity * 2 - 1] = -1;
  this->insert = this->size;
  this->last = this->capacity - 1;

  // Manual copy
  auto* src_cursor = other.ptr + LAYER_HEADER_SIZE * other.capacity; // NOLINT
  auto* dst_cursor = this->ptr + LAYER_HEADER_SIZE * this->capacity; // NOLINT

  for (i32 i = 0, count = 0; count < this->size;
       ++i, src_cursor += other.bytes) {
    // NOTE: Way to check for inf loops
    assert(src_cursor - other.ptr <= other.allocated_size);

    if (((u32*)other.ptr)[i] == 0U) {
      continue;
    }

    ((u32*)this->ptr)[count] = ((u32*)other.ptr)[i];
    std::memcpy(dst_cursor, src_cursor, other.bytes);

    dst_cursor += this->bytes;
    ++count;
  }
}

void Anim::LayerArena::copy_grow(const LayerArena& other) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr = (data_ptr)std::realloc(this->ptr, other.allocated_size);
  if (new_ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }
  this->ptr = new_ptr;

  this->copy_normal(other);
}

void Anim::LayerArena::minicopy(const LayerArena& other) noexcept {
  if (this == &other) {
    return;
  }

  // TODO: Reuse memory if already allocated
  if (this->ptr) {
    std::free(this->ptr); // NOLINT
  }

  // Create the minified copy of the layer arena class
  this->insert = this->last = -1;
  this->size = this->capacity = other.size;
  this->bytes = other.bytes;
  this->allocated_size = other.size * (other.bytes + LAYER_HEADER_SIZE);

  // NOLINTNEXTLINE
  this->ptr = (data_ptr)std::malloc(this->allocated_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }

  // Manual Copy
  auto* src_cursor = other.ptr + LAYER_HEADER_SIZE * other.capacity; // NOLINT
  auto* dst_cursor = this->ptr + LAYER_HEADER_SIZE * this->size;     // NOLINT

  for (i32 i = 0, count = 0; count < this->size;
       ++i, src_cursor += other.bytes) {
    // NOTE: Way to check for inf loops
    assert(src_cursor - other.ptr <= other.allocated_size);

    if (((u32*)other.ptr)[i] == 0U) {
      continue;
    }

    ((u32*)this->ptr)[count] = ((u32*)other.ptr)[i];
    std::memcpy(dst_cursor, src_cursor, other.bytes);

    dst_cursor += this->bytes;
    ++count;
  }
}

Anim::LayerArena::~LayerArena() noexcept {
  if (this->ptr) {
    std::free(this->ptr); // NOLINT
    this->ptr = nullptr;
  }
}

// === Getters / Accessors === //

data_ptr Anim::LayerArena::get_ptr() const noexcept {
  return this->ptr;
}

data_ptr Anim::LayerArena::get_layer(u32 id) const noexcept {
  assert(this->ptr != nullptr);

  for (i32 i = 0; i < this->capacity; ++i) {
    if (((u32*)this->ptr)[i] != id) {
      continue;
    }

    // NOLINTNEXTLINE
    return this->ptr + this->capacity * LAYER_HEADER_SIZE + i * this->bytes;
  }

  return nullptr;
}

// === Modifiers === //

void Anim::LayerArena::create_layer(u32 new_id) noexcept {
  // TODO: Try to invalidate a frame
  assert(this->ptr != nullptr);
  assert(this->insert != -1);

  ((u32*)this->ptr)[this->insert] = new_id;
  this->insert = ((i32*)this->ptr)[this->capacity + this->insert];
  ++this->size;
}

// === Memory === //

void Anim::LayerArena::allocate(i32 allocated_size) noexcept {
  // NOLINTNEXTLINE
  this->ptr = (data_ptr)std::malloc(allocated_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }
  this->allocated_size = allocated_size;

  // Set all ids to 0
  std::memset(this->ptr, 0, this->capacity * sizeof(u32));
  *(u32*)this->ptr = 1U;
  this->size = 1;

  // Set the values of the next id
  for (i32 i = 0; i < this->capacity; ++i) {
    ((i32*)this->ptr)[i + this->capacity] = i + 1;
  }
  ((i32*)this->ptr)[this->capacity * 2 - 1] = -1;
  this->insert = 1;
  this->last = this->capacity - 1;

  // Set the layer data values to 0
  std::memset(
      // NOLINTNEXTLINE
      this->ptr + LAYER_HEADER_SIZE * this->capacity, 0,
      this->allocated_size - LAYER_HEADER_SIZE * this->capacity
  );
}

} // namespace draw

