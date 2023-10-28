/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-05
 *==========================*/

#include "./anim.hpp"
#include "math.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace draw {

const i32 FRAME_CAPACITY_START = 4;
const i32 LAYER_CAPACITY_START = 4;

// NOTE: Too many multiplication, chance of having an overflow somewhere
// TODO: Implement a b+ tree instead for this
void Anim::init(ivec size, ColorType type) noexcept {
  this->type = type;
  this->size = size;
  this->frame_count = this->layer_count = 1;
  this->frame_capacity = FRAME_CAPACITY_START;
  this->layer_capacity = LAYER_CAPACITY_START;

  this->next_layer = size.x * size.y * this->get_datatype_size();
  this->next_frame = this->next_layer * LAYER_CAPACITY_START;
  this->max_size =
      FRAME_CAPACITY_START * LAYER_CAPACITY_START * this->next_layer;
  // NOLINTNEXTLINE
  this->ptr = (data_ptr)std::calloc(this->max_size, 1);
  if (this->ptr == nullptr) {
    // TODO: return an error instead
    std::abort();
  }
}

void Anim::copy(const Anim& other) noexcept {
  // Reuse the size
  if (!this->ptr) {
    // NOLINTNEXTLINE
    this->ptr = (data_ptr)std::calloc(other.max_size, 1);
    if (this->ptr == nullptr) {
      // TODO: return an error instead
      std::abort();
    }
  } else if (this->max_size < other.max_size) {
    // NOLINTNEXTLINE
    auto new_ptr = (data_ptr)std::realloc(this->ptr, other.max_size);
    if (new_ptr == nullptr) {
      // TODO: return an error instead
      std::abort();
    }
    this->ptr = new_ptr;
  }

  std::memcpy(this->ptr, other.ptr, other.max_size);
  this->max_size = other.max_size;

  this->frame_capacity = other.frame_capacity;
  this->frame_count = other.frame_count;
  this->next_frame = other.next_frame;

  this->layer_capacity = other.layer_capacity;
  this->layer_count = other.layer_count;
  this->next_layer = other.next_layer;

  this->type = other.type;
  this->size = other.size;
}

Anim::Anim(Anim&& rhs) noexcept
    : ptr(rhs.ptr),
      max_size(rhs.max_size),
      frame_capacity(rhs.frame_capacity),
      frame_count(rhs.frame_count),
      next_frame(rhs.next_frame),
      layer_capacity(rhs.layer_capacity),
      layer_count(rhs.layer_count),
      next_layer(rhs.next_layer),
      type(rhs.type),
      size(rhs.size) {
  rhs.ptr = nullptr;
}

Anim& Anim::operator=(Anim&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->ptr = rhs.ptr;
  rhs.ptr = nullptr;

  this->max_size = rhs.max_size;

  this->frame_capacity = rhs.frame_capacity;
  this->frame_count = rhs.frame_count;
  this->next_frame = rhs.next_frame;

  this->layer_capacity = rhs.layer_capacity;
  this->layer_count = rhs.layer_count;
  this->next_layer = rhs.next_layer;

  this->type = rhs.type;
  this->size = rhs.size;

  return *this;
}

Anim::~Anim() noexcept {
  if (this->ptr) {
    // NOLINTNEXTLINE
    std::free(this->ptr);
    this->ptr = nullptr;
  }
}

data_ptr Anim::get_ptr() noexcept {
  return this->ptr;
}

ColorType Anim::get_type() const noexcept {
  return this->type;
}

i32 Anim::get_frame_count() const noexcept {
  return this->frame_count;
}

i32 Anim::get_layer_count() const noexcept {
  return this->layer_count;
}

i32 Anim::get_frame_capacity() const noexcept {
  return this->frame_capacity;
}

i32 Anim::get_layer_capacity() const noexcept {
  return this->layer_capacity;
}

ivec Anim::get_size() const noexcept {
  return this->size;
}

i32 Anim::get_width() const noexcept {
  return this->size.x;
}

i32 Anim::get_height() const noexcept {
  return this->size.y;
}

bool Anim::has_point(ivec point) const noexcept {
  return point.x >= 0 && point.x < this->size.x && point.y >= 0 &&
         point.y < this->size.y;
}

void Anim::clear() noexcept {
  if (this->ptr) {
    // NOLINTNEXTLINE
    std::free(this->ptr);
    this->ptr = nullptr;
  }

  this->max_size = 0;
  this->frame_capacity = 0;
  this->frame_count = 0;
  this->next_layer = 0;
  this->layer_capacity = 0;
  this->layer_count = 0;
  this->next_layer = 0;
  this->type = ColorType::NONE;
}

i32 Anim::get_datatype_size() const noexcept {
  return 0x0000'ffff & this->type;
}

void Anim::insert_frame(i32 index) noexcept {
  assert(index >= 0 && index <= this->frame_count);

  if (this->frame_count == this->frame_capacity) {
    // TODO: May fail due to bad alloc
    this->resize_frame(math::get_next_pow2(this->frame_capacity));
  }

  // NOLINTNEXTLINE
  auto* cursor = this->ptr + index * this->next_frame;
  i32 shift = (this->frame_count - index) * this->next_frame;
  std::memmove(cursor + this->next_frame, cursor, shift); // Rotate
  std::memset(cursor, 0, this->next_frame);               // Clear

  ++this->frame_count;
}

void Anim::insert_layer(i32 index) noexcept {
  assert(index >= 0 && index <= this->layer_count);

  if (this->layer_count == this->layer_capacity) {
    // TODO: May fail due to bad alloc
    this->resize_layer(math::get_next_pow2(this->layer_capacity));
  }

  // NOLINTNEXTLINE
  auto* cursor = this->ptr + index * this->next_layer;
  i32 shift = (this->layer_count - index) * this->next_layer;
  // Loop thru all the frames to add the layer
  for (i32 i = 0; i < this->frame_count; ++i) {
    std::memmove(cursor + this->next_layer, cursor, shift); // Rotate
    std::memset(cursor, 0, this->next_layer);               // Clear
    cursor += this->next_frame;
  }

  ++this->layer_count;
}

void Anim::insert_frames(i32 index, i32 count) noexcept {
  assert(index >= 0 && index <= this->frame_count);

  if (this->frame_count + count > this->frame_capacity) {
    // TODO: May fail due to bad alloc
    this->resize_frame(math::get_next_pow2(this->frame_count + count));
  }

  // NOLINTNEXTLINE
  auto* cursor = this->ptr + index * this->next_frame;
  i32 shift = (this->frame_count - index) * this->next_frame;
  i32 size = count * this->next_frame;
  std::memmove(cursor + size, cursor, shift);
  std::memset(cursor, 0, size);

  this->frame_count += count;
}

void Anim::insert_layers(i32 index, i32 count) noexcept {
  assert(index >= 0 && index <= this->layer_count);

  if (this->layer_count + count > this->layer_capacity) {
    // TODO: May fail due to bad alloc
    this->resize_layer(math::get_next_pow2(this->layer_count + count));
  }

  // NOLINTNEXTLINE
  auto* cursor = this->ptr + index * this->next_layer;
  i32 shift = (this->layer_count - index) * this->next_layer;
  i32 size = count * this->next_layer;
  // Loop thru all the frames to add the layer
  for (i32 i = 0; i < this->frame_count; ++i) {
    std::memmove(cursor + size, cursor, shift); // Rotate
    std::memset(cursor, 0, size);               // Clear

    cursor += this->next_frame;
  }

  this->layer_count += count;
}

void Anim::resize_frame(i32 new_frame_capacity) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr = (data_ptr)std::realloc(
      // NOLINTNEXTLINE
      this->ptr, new_frame_capacity * this->layer_capacity * this->next_layer
  );

  if (!new_ptr) {
    // TODO: Handle bad alloc
    return;
  }

  if (new_ptr != this->ptr) {
    this->ptr = new_ptr;
  }

  // No need to shift the data, data is already placed correctly

  this->frame_capacity = new_frame_capacity;
}

void Anim::resize_layer(i32 new_layer_capacity) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr = (data_ptr)std::realloc(
      // NOLINTNEXTLINE
      this->ptr, new_layer_capacity * this->frame_capacity * this->next_layer
  );

  if (!new_ptr) {
    // TODO: Handle bad alloc
    return;
  }

  if (new_ptr != this->ptr) {
    this->ptr = new_ptr;
  }

  // Shift the data
  i32 shift = this->layer_count * this->next_layer;
  i32 new_next_frame = this->next_layer * new_layer_capacity;
  // NOLINTNEXTLINE
  auto* src_cursor = this->ptr + (this->frame_count - 1) * this->next_frame;
  // NOLINTNEXTLINE
  auto* dst_cursor = this->ptr + (this->frame_count - 1) * new_next_frame;
  while (src_cursor > this->ptr) {
    std::memmove(dst_cursor, src_cursor, shift);
    std::memset(src_cursor, 0, shift);
  }

  this->next_frame = new_next_frame;
  this->layer_capacity = new_layer_capacity;
}

void Anim::print() const noexcept {
  auto* curr_ptr = this->ptr;
  for (i32 f = 0; f < this->frame_count; ++f) {
    for (i32 y = 0; y < this->size.y; ++y) {
      printf("%3dy: ", y);
      for (i32 x = 0; x < this->size.x; ++x) {
        printf(
            "%08X ",
            // NOLINTNEXTLINE
            curr_ptr[(x + y * this->size.x) * this->get_datatype_size()]
        );
      }
      printf("\n");
    }
    printf("=== \n");
    curr_ptr += this->next_frame;
  }
}

} // namespace draw

