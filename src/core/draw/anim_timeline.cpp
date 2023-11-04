/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-31
 *==========================*/

#include "./anim.hpp"
#include "core/logger/logger.hpp"
#include "math.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>

inline const i32 LAYER_INIT = 4;
// User might only create a graphic without an animation
inline const i32 FRAME_INIT = 1;
inline const u8 OPACITY_BIT = 0x80;

// NOTE: Can be stores in locale
c8 layer_prefix[std::strlen("Layer 2147483647") + 1U] = "Layer "; // NOLINT
const c8* get_default_name(i32 index) noexcept {
  assert(index > 0);

  // 2147483647
  c8 memo[10]; // NOLINT
  i32 n = 0;
  while (index > 0) {
    memo[n] = index % 10;
    index /= 10;
    ++n;
  }

  i32 off = std::strlen("Layer ");
  for (n = n - 1; n >= 0; --n, ++off) {
    layer_prefix[off] = memo[n] + '0';
  }
  layer_prefix[off] = '\0';

  return layer_prefix;
}

namespace draw {

void TimelineInfo::init() noexcept {
  this->layer_capacity = LAYER_INIT;
  this->frame_capacity = FRAME_INIT;
  // +1 is the frame id
  this->allocate_timeline(
      (this->layer_capacity + 1) * this->frame_capacity * (i32)sizeof(u32)
  );
  this->timeline[0] = 1U;
  this->timeline[this->frame_capacity] = 1U;
  this->layer_count = this->frame_count = 1;

  this->allocate_layer_info(this->layer_capacity * sizeof(LayerInfo));

  // Safe copy
  std::strcpy(this->layer_info->name, "Layer 1");
  // visible bit and 100 opacity
  this->layer_info->opacity = OPACITY_BIT | 100U;
}

TimelineInfo::TimelineInfo(TimelineInfo&& rhs) noexcept
    : timeline(rhs.timeline),
      layer_info(rhs.layer_info),
      layer_count(rhs.layer_count),
      frame_count(rhs.frame_count),
      layer_capacity(rhs.layer_capacity),
      timeline_alloc_size(rhs.timeline_alloc_size),
      layer_info_alloc_size(rhs.layer_info_alloc_size) {
  rhs.timeline = nullptr;
  rhs.layer_info = nullptr;
}

TimelineInfo& TimelineInfo::operator=(TimelineInfo&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->timeline = rhs.timeline;
  rhs.timeline = nullptr;

  this->layer_info = rhs.layer_info;
  rhs.layer_info = nullptr;

  this->layer_count = rhs.layer_count;
  this->frame_count = rhs.frame_count;
  this->layer_capacity = rhs.layer_capacity;
  this->timeline_alloc_size = rhs.timeline_alloc_size;
  this->layer_info_alloc_size = rhs.layer_info_alloc_size;

  return *this;
}

// === Copy === //

void TimelineInfo::copy(const TimelineInfo& other) noexcept {
  if (this == &other) {
    return;
  }

  if (this->timeline == nullptr) {
    this->copy_empty(other);
  } else if (this->timeline_alloc_size == other.timeline_alloc_size) {
    this->copy_normal(other);
  } else if (this->timeline_alloc_size > other.timeline_alloc_size) {
    this->copy_overfit(other);
  } else {
    this->copy_grow(other);
  }

  // TODO: Minify copy here
  if (this->layer_info) {
    std::free(this->layer_info); // NOLINT
  }

  // NOLINTNEXTLINE
  this->layer_info = (LayerInfo*)std::malloc(other.layer_info_alloc_size);
  if (this->layer_info == nullptr) {
    logger::fatal("Could not allocate animation layer_info");
    std::abort();
  }

  this->layer_info_alloc_size = other.layer_info_alloc_size;
  std::memcpy(this->layer_info, other.layer_info, this->layer_info_alloc_size);
}

void TimelineInfo::copy_empty(const TimelineInfo& other) noexcept {
  // NOLINTNEXTLINE
  this->timeline = (u32*)std::malloc(other.timeline_alloc_size);
  if (this->timeline == nullptr) {
    logger::fatal("Could not allocate animation frames");
    std::abort();
  }

  this->copy_normal(other);
}

void TimelineInfo::copy_normal(const TimelineInfo& other) noexcept {
  std::memcpy(this->timeline, other.timeline, other.timeline_alloc_size);

  this->layer_count = other.layer_count;
  this->frame_count = other.frame_count;
  this->layer_capacity = other.layer_capacity;
  this->timeline_alloc_size = other.timeline_alloc_size;
}

void TimelineInfo::copy_overfit(const TimelineInfo& other) noexcept {
  this->layer_count = other.layer_count;
  this->frame_count = other.frame_count;
  this->frame_capacity = other.frame_capacity;
  this->layer_capacity =
      this->timeline_alloc_size / (this->frame_capacity * (i32)sizeof(u32)) - 1;

  std::memcpy(this->timeline, other.timeline, this->frame_count * sizeof(u32));

  auto* src_cursor = other.timeline + this->frame_capacity;
  auto* dst_cursor = this->timeline + this->frame_capacity;
  for (i32 i = 0; i < this->frame_count; ++i) {
    std::memcpy(dst_cursor, src_cursor, this->layer_count * sizeof(u32));
    src_cursor += other.layer_capacity;
    dst_cursor += this->layer_capacity;
  }
}

void TimelineInfo::copy_grow(const TimelineInfo& other) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr = (u32*)std::realloc(this->timeline, other.timeline_alloc_size);
  if (new_ptr == nullptr) {
    logger::fatal("Could not allocate animation timeline");
    std::abort();
  }
  this->timeline = new_ptr;

  this->copy_normal(other);
}

TimelineInfo::~TimelineInfo() noexcept {
  if (this->timeline == nullptr) {
    return;
  }

  std::free(this->timeline); // NOLINT
  this->timeline = nullptr;

  std::free(this->layer_info); // NOLINT
  this->layer_info = nullptr;
}

// === Getters / Accessors === //

u32 TimelineInfo::get_image_id(i32 frame_id, i32 layer_index) const noexcept {
  assert(frame_id != 0U && layer_index < this->layer_count);

  if (frame_id == 1) {
    return this->timeline[this->frame_capacity + layer_index];
  }

  i32 i = 1;
  for (; i < this->frame_count; ++i) {
    if (this->timeline[i] >= frame_id) {
      break;
    }
  }

  return this->timeline
      [this->frame_capacity + this->layer_capacity * (i - 1) + layer_index];
}

i32 TimelineInfo::get_layer_count() const noexcept {
  return this->layer_count;
}

i32 TimelineInfo::get_layer_capacity() const noexcept {
  return this->layer_capacity;
}

const c8* TimelineInfo::get_layer_name(i32 index) const noexcept {
  assert(index >= 0 && index < this->layer_count);
  return this->layer_info[index].name; // NOLINT
}

bool TimelineInfo::is_layer_visible(i32 index) const noexcept {
  assert(index >= 0 && index < this->layer_count);
  return this->layer_info[index].opacity & OPACITY_BIT;
}

i32 TimelineInfo::get_timeline_alloc_size() const noexcept {
  return this->timeline_alloc_size;
}

Frame TimelineInfo::get_frame(u32 id) const noexcept {
  assert(id != 0U);

  if (id == 1) {
    return Frame{this->timeline + this->frame_capacity, this->layer_count, 1U};
  }

  i32 i = 1;
  for (; i < this->frame_count; ++i) {
    if (this->timeline[i] >= id) {
      break;
    }
  }

  --i;
  return Frame{// NOLINTNEXTLINE
               this->timeline + this->frame_capacity + this->layer_capacity * i,
               this->layer_count, this->timeline[i]};
}

// === Modifiers === //

void TimelineInfo::insert_layer(i32 index, u32 layer_id) noexcept {
  assert(index >= 0 && index <= this->layer_count);

  if (this->layer_count >= this->layer_capacity) {
    i32 new_size = math::get_next_pow2(this->layer_count);
    this->reallocate_timeline_on_layer(new_size);
    this->reallocate_layer_info(new_size * sizeof(LayerInfo));
  }

  auto* cursor = this->timeline + this->frame_capacity + index;
  for (i32 i = 0; i < this->frame_count; ++i) {
    std::memmove(cursor + 1, cursor, (this->layer_count - index) * sizeof(i32));
    *cursor = 0U;
    cursor += this->layer_capacity;
  }
  this->timeline[this->frame_capacity + index] = layer_id;

  std::memmove(
      this->layer_info + index + 1, this->layer_info + index,
      sizeof(LayerInfo) * (this->layer_count - index) // NOLINT
  );
  std::strcpy(
      this->layer_info[index].name, get_default_name(this->layer_count + 1)
  );
  this->layer_info[index].opacity = OPACITY_BIT | 100U;

  ++this->layer_count;
}

bool TimelineInfo::toggle_layer_visibility(i32 index) noexcept {
  assert(index >= 0 && index < this->layer_count);

  auto& info = this->layer_info[index].opacity;
  info ^= OPACITY_BIT;
  return info &= OPACITY_BIT;
}

// === Memory === //

void TimelineInfo::allocate_timeline(i32 new_size) noexcept {
  this->timeline_alloc_size = new_size;
  // NOLINTNEXTLINE
  this->timeline = (u32*)std::malloc(this->timeline_alloc_size);
  if (this->timeline == nullptr) {
    logger::fatal("Could not allocate animation timeline");
    std::abort();
  }
}

void TimelineInfo::reallocate_timeline_on_layer(i32 new_layer_capacity
) noexcept {
  auto new_capacity =
      (new_layer_capacity + 1) * this->frame_capacity * (i32)sizeof(u32);
  auto* new_ptr = (u32*)std::realloc(this->timeline, new_capacity); // NOLINT
  if (new_ptr == nullptr) {
    logger::fatal("Could not allocate animation timeline");
    std::abort();
  }

  this->timeline = new_ptr;

  auto* dst_cursor =
      // NOLINTNEXTLINE
      this->timeline + this->frame_capacity +
      (this->frame_count - 1) * new_layer_capacity;
  auto* src_cursor =
      // NOLINTNEXTLINE
      this->timeline + this->frame_capacity +
      (this->frame_count - 1) * this->layer_capacity;
  for (i32 i = 1; i < this->frame_count; ++i) {
    std::memmove(dst_cursor, src_cursor, this->layer_count * sizeof(u32));
    dst_cursor -= new_layer_capacity;
    src_cursor -= this->layer_capacity;
  }

  this->layer_capacity = new_layer_capacity;
  this->timeline_alloc_size = new_capacity;
}

void TimelineInfo::allocate_layer_info(i32 new_size) noexcept {
  this->layer_info_alloc_size = new_size;
  // NOLINTNEXTLINE
  this->layer_info = (LayerInfo*)std::malloc(new_size);
  if (this->layer_info == nullptr) {
    logger::fatal("Could not allocate animation layer_info");
    std::abort();
  }
}

void TimelineInfo::reallocate_layer_info(i32 new_size) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr =
      // NOLINTNEXTLINE
      (LayerInfo*)std::realloc(this->layer_info, new_size);
  if (new_ptr == nullptr) {
    logger::fatal("Could not allocate animation layer_info");
    std::abort();
  }
  this->layer_info = new_ptr;
}

} // namespace draw

