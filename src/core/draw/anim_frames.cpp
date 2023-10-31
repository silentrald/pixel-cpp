/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-31
 *==========================*/

#include "./anim.hpp"
#include "core/logger/logger.hpp"
#include "math.hpp"
#include <cstdlib>
#include <cstring>

inline const i32 LAYER_INIT = 4;
// User might only create a graphic without an animation
inline const i32 FRAME_INIT = 1;

namespace draw {

void Anim::FrameArena::init() noexcept {
  this->layer_capacity = LAYER_INIT;
  this->frame_capacity = FRAME_INIT;
  // +1 is the frame id
  this->allocate(
      (this->layer_capacity + 1) * this->frame_capacity * (i32)sizeof(u32)
  );
  this->ptr[0] = 1U;
  this->ptr[this->frame_capacity] = 1U;
  this->layer_count = this->frame_count = 1;
}

Anim::FrameArena::FrameArena(FrameArena&& rhs) noexcept
    : ptr(rhs.ptr),
      layer_count(rhs.layer_count),
      frame_count(rhs.frame_count),
      layer_capacity(rhs.layer_capacity),
      allocated_size(rhs.allocated_size) {
  rhs.ptr = nullptr;
}

Anim::FrameArena& Anim::FrameArena::operator=(FrameArena&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->ptr = rhs.ptr;
  rhs.ptr = nullptr;

  this->layer_count = rhs.layer_count;
  this->frame_count = rhs.frame_count;
  this->layer_capacity = rhs.layer_capacity;
  this->allocated_size = rhs.allocated_size;

  return *this;
}

// === Copy === //

void Anim::FrameArena::copy(const FrameArena& other) noexcept {
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

void Anim::FrameArena::copy_empty(const FrameArena& other) noexcept {
  // NOLINTNEXTLINE
  this->ptr = (u32*)std::malloc(other.allocated_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation frames");
    std::abort();
  }

  this->copy_normal(other);
}

void Anim::FrameArena::copy_normal(const FrameArena& other) noexcept {
  std::memcpy(this->ptr, other.ptr, other.allocated_size);

  this->layer_count = other.layer_count;
  this->frame_count = other.frame_count;
  this->layer_capacity = other.layer_capacity;
  this->allocated_size = other.allocated_size;
}

void Anim::FrameArena::copy_overfit(const FrameArena& other) noexcept {
  this->layer_count = other.layer_count;
  this->frame_count = other.frame_count;
  this->frame_capacity = other.frame_capacity;
  this->layer_capacity =
      this->allocated_size / (this->frame_capacity * (i32)sizeof(u32)) - 1;

  std::memcpy(this->ptr, other.ptr, this->frame_count * sizeof(u32));

  auto* src_cursor = other.ptr + this->frame_capacity;
  auto* dst_cursor = this->ptr + this->frame_capacity;
  for (i32 i = 0; i < this->frame_count; ++i) {
    std::memcpy(dst_cursor, src_cursor, this->layer_count * sizeof(u32));
    src_cursor += other.layer_capacity;
    dst_cursor += this->layer_capacity;
  }
}

void Anim::FrameArena::copy_grow(const FrameArena& other) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr = (u32*)std::realloc(this->ptr, other.allocated_size);
  if (new_ptr == nullptr) {
    logger::fatal("Could not allocate animation layers");
    std::abort();
  }
  this->ptr = new_ptr;

  this->copy_normal(other);
}

Anim::FrameArena::~FrameArena() noexcept {
  if (this->ptr) {
    std::free(this->ptr); // NOLINT
    this->ptr = nullptr;
  }
}

// === Getters / Accessors === //

i32 Anim::FrameArena::get_layer_count() const noexcept {
  return this->layer_count;
}

i32 Anim::FrameArena::get_layer_capacity() const noexcept {
  return this->layer_capacity;
}

u32 Anim::FrameArena::get_layer_id(i32 frame_id, i32 layer_index)
    const noexcept {
  assert(frame_id != 0U && layer_index < this->layer_count);

  if (frame_id == 1) {
    return this->ptr[this->frame_capacity + layer_index];
  }

  for (i32 i = 1; i < this->frame_count; ++i) {
    if (this->ptr[i] < frame_id) {
      continue;
    }

    return this->ptr
        [this->frame_capacity + this->layer_capacity * (i - 1) + layer_index];
  }

  return this->ptr
      [this->frame_capacity + this->layer_capacity * (this->layer_count - 1) +
       layer_index];
}

i32 Anim::FrameArena::get_allocated_size() const noexcept {
  return this->allocated_size;
}

// === Modifiers === //

void Anim::FrameArena::insert_layer(i32 index, u32 layer_id) noexcept {
  if (this->layer_count >= this->layer_capacity) {
    this->reallocate_on_layer(math::get_next_pow2(this->layer_count));
  }

  auto* cursor = this->ptr + this->frame_capacity + index;
  for (i32 i = 0; i < this->frame_count; ++i) {
    std::memmove(cursor + 1, cursor, (this->layer_count - index) * sizeof(i32));
    *cursor = 0U;
    cursor += this->layer_capacity;
  }
  this->ptr[this->frame_capacity + index] = layer_id;
  ++this->layer_count;
}

// === Memory === //

void Anim::FrameArena::allocate(i32 new_size) noexcept {
  this->allocated_size = new_size;
  // NOLINTNEXTLINE
  this->ptr = (u32*)std::malloc(this->allocated_size);
  if (this->ptr == nullptr) {
    logger::fatal("Could not allocate animation frames");
    std::abort();
  }
}

void Anim::FrameArena::reallocate_on_layer(i32 new_layer_capacity) noexcept {
  auto new_capacity =
      (new_layer_capacity + 1) * this->frame_capacity * (i32)sizeof(u32);
  auto* new_ptr = (u32*)std::realloc(this->ptr, new_capacity); // NOLINT
  if (new_ptr == nullptr) {
    logger::fatal("Could not allocate animation frames");
    std::abort();
  }

  this->ptr = new_ptr;

  auto* dst_cursor =
      // NOLINTNEXTLINE
      this->ptr + this->frame_capacity +
      (this->frame_count - 1) * new_layer_capacity;
  auto* src_cursor =
      // NOLINTNEXTLINE
      this->ptr + this->frame_capacity +
      (this->frame_count - 1) * this->layer_capacity;
  for (i32 i = 1; i < this->frame_count; ++i) {
    std::memmove(dst_cursor, src_cursor, this->layer_count * sizeof(u32));
    dst_cursor -= new_layer_capacity;
    src_cursor -= this->layer_capacity;
  }

  this->layer_capacity = new_layer_capacity;
  this->allocated_size = new_capacity;
}

} // namespace draw

