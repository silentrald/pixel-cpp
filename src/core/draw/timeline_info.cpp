/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-31
 *==========================*/

#include "./timeline_info.hpp"
#include "core/logger/logger.hpp"
#include "math.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <utility>

namespace draw {

inline const usize LAYER_INIT = 4U;
// User might only create a graphic without an animation
inline const usize FRAME_INIT = 1U;
inline const u8 VISIBLE_BIT = 0x80U;

// NOTE: Can be stores in locale
c8 layer_prefix[std::strlen("Layer 4294967295 ")] = "Layer "; // NOLINT
const c8* get_default_name(usize index) noexcept {
  assert(index > 0U);

  // 4294967295
  c8 memo[std::strlen("4294967295")]; // NOLINT
  usize n = 0U;
  while (index > 0U) {
    memo[n] = index % 10U;
    index /= 10U;
    ++n;
  }

  usize off = std::strlen("Layer ");
  for (; n > 0U; --n, ++off) {
    layer_prefix[off] = memo[n - 1U] + '0';
  }
  layer_prefix[off] = '\0';

  return layer_prefix;
}

error_code TimelineInfo::init() noexcept {
  TRY(this->allocate_by_capacities(LAYER_INIT, FRAME_INIT));

  this->layer_capacity = LAYER_INIT;
  this->frame_capacity = FRAME_INIT;

  // Set first frame id to 1U
  *this->get_frame_id_ptr() = 1U;
  // Set the id image to 1U, the first created image
  *this->get_image_id_ptr() = 1U;
  this->layer_count = this->frame_count = 1U;

  // Safe copy
  std::strcpy(this->layer_info->name, "Layer 1");
  // visible bit and 100 opacity
  this->layer_info->opacity = VISIBLE_BIT | 100U;

  return error_code::OK;
}

error_code
TimelineInfo::load_init(usize layer_count, usize frame_count) noexcept {
  TRY(this->allocate_by_capacities(
      math::get_next_pow2(layer_count),
      frame_count == 1U ? 1U : math::get_next_pow2(frame_count)
  ));

  this->layer_count = layer_count;
  this->layer_capacity = layer_count;
  this->frame_count = frame_count;
  this->frame_capacity = frame_count;

  return error_code::OK;
}

void TimelineInfo::load_layer(usize index, LayerInfo layer_info) noexcept {
  this->layer_info[index] = layer_info;
}

void TimelineInfo::load_frame(
    usize index, usize id, usize* image_ids
) noexcept {
  this->get_frame_id_ptr()[index] = id;
  std::memcpy(
      this->get_image_id_ptr(index), image_ids,
      this->layer_count * sizeof(usize)
  );
}

TimelineInfo::TimelineInfo(TimelineInfo&& rhs) noexcept
    : timeline(rhs.timeline),
      layer_info(rhs.layer_info),
      layer_count(rhs.layer_count),
      frame_count(rhs.frame_count),
      layer_capacity(rhs.layer_capacity),
      frame_capacity(rhs.frame_capacity),
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
  this->frame_capacity = rhs.frame_capacity;
  this->timeline_alloc_size = rhs.timeline_alloc_size;
  this->layer_info_alloc_size = rhs.layer_info_alloc_size;

  return *this;
}

// === Copy === //

error_code TimelineInfo::copy(const TimelineInfo& other) noexcept {
  if (this == &other) {
    return error_code::OK;
  }

  if (this->timeline == nullptr) {
    TRY(this->copy_empty(other));
  } else if (this->timeline_alloc_size == other.timeline_alloc_size) {
    this->copy_normal(other);
  } else if (this->timeline_alloc_size > other.timeline_alloc_size) {
    this->copy_overfit(other);
  } else {
    TRY(this->copy_grow(other));
  }

  // TODO: Minify copy here
  if (this->layer_info) {
    std::free(this->layer_info); // NOLINT
  }

  // NOLINTNEXTLINE
  this->layer_info = (LayerInfo*)std::malloc(other.layer_info_alloc_size);
  if (this->layer_info == nullptr) {
    return error_code::BAD_ALLOC;
  }

  this->layer_info_alloc_size = other.layer_info_alloc_size;
  std::memcpy(this->layer_info, other.layer_info, this->layer_info_alloc_size);

  return error_code::OK;
}

error_code TimelineInfo::copy_empty(const TimelineInfo& other) noexcept {
  TRY(this->allocate_timeline(other.timeline_alloc_size));
  this->copy_normal(other);
  return error_code::OK;
}

void TimelineInfo::copy_normal(const TimelineInfo& other) noexcept {
  std::memcpy(this->timeline, other.timeline, other.timeline_alloc_size);

  this->layer_count = other.layer_count;
  this->frame_count = other.frame_count;
  this->layer_capacity = other.layer_capacity;
  this->frame_capacity = other.frame_capacity;
}

void TimelineInfo::copy_overfit(const TimelineInfo& other) noexcept {
  this->layer_count = other.layer_count;
  this->frame_count = other.frame_count;
  this->frame_capacity = other.frame_capacity;
  this->layer_capacity =
      this->timeline_alloc_size / (this->frame_capacity * sizeof(usize)) - 1;

  std::memcpy(
      this->timeline, other.timeline, this->frame_count * sizeof(usize)
  );

  auto* src_cursor = other.get_image_id_ptr();
  auto* dst_cursor = this->get_image_id_ptr();
  for (usize i = 0U; i < this->frame_count; ++i) {
    std::memcpy(dst_cursor, src_cursor, this->layer_count * sizeof(usize));
    src_cursor += other.layer_capacity;
    dst_cursor += this->layer_capacity;
  }
}

error_code TimelineInfo::copy_grow(const TimelineInfo& other) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr =
      // NOLINTNEXTLINE
      (usize*)std::realloc(this->timeline, other.timeline_alloc_size);
  if (new_ptr == nullptr) {
    return error_code::BAD_ALLOC;
  }
  this->timeline = new_ptr;
  this->timeline_alloc_size = other.timeline_alloc_size;

  this->copy_normal(other);
  return error_code::OK;
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

usize TimelineInfo::get_image_id(usize frame_id, usize layer_index)
    const noexcept {
  assert(frame_id != 0U && layer_index < this->layer_count);

  if (frame_id == 1U) {
    return this->get_image_id_ptr()[layer_index];
  }

  usize i = 1U;
  for (; i < this->frame_count; ++i) {
    if (this->get_frame_id(i) >= frame_id) {
      break;
    }
  }

  return this->get_image_id_ptr(i - 1U)[layer_index];
}

usize TimelineInfo::get_layer_count() const noexcept {
  return this->layer_count;
}

usize TimelineInfo::get_frame_count() const noexcept {
  return this->frame_count;
}

usize TimelineInfo::get_layer_capacity() const noexcept {
  return this->layer_capacity;
}

const LayerInfo& TimelineInfo::get_layer_info(usize index) const noexcept {
  return this->layer_info[index];
}

const c8* TimelineInfo::get_layer_name(usize index) const noexcept {
  assert(index >= 0 && index < this->layer_count);
  return this->layer_info[index].name; // NOLINT
}

bool TimelineInfo::is_layer_visible(usize index) const noexcept {
  assert(index >= 0 && index < this->layer_count);
  return this->layer_info[index].opacity & VISIBLE_BIT;
}

usize TimelineInfo::get_timeline_alloc_size() const noexcept {
  return this->timeline_alloc_size;
}

Frame TimelineInfo::get_frame(usize id) const noexcept {
  assert(id != 0U);

  if (id == 1U) {
    return Frame{this->get_image_id_ptr(), this->layer_count, 1U};
  }

  usize i = 1U;
  for (; i < this->frame_count; ++i) {
    if (this->get_frame_id(i) >= id) {
      break;
    }
  }

  --i;
  return Frame{
      this->get_image_id_ptr(i), this->layer_count, this->get_frame_id(i)};
}

FrameIter TimelineInfo::get_frame_iter() const noexcept {
  return {
      this->timeline, this->frame_count, this->frame_capacity,
      this->layer_capacity};
}

// === Modifiers === //

error_code TimelineInfo::insert_layer(usize index, usize layer_id) noexcept {
  assert(index >= 0 && index <= this->layer_count);

  if (this->layer_count >= this->layer_capacity) {
    usize new_size = math::get_next_pow2(this->layer_count);
    TRY(this->reallocate_timeline_on_layer(new_size));
    TRY(this->reallocate_layer_info(new_size * sizeof(LayerInfo)));
  }

  auto* cursor = this->get_image_id_ptr() + index;
  for (usize i = 0U; i < this->frame_count; ++i) {
    std::memmove(
        cursor + 1, cursor, (this->layer_count - index) * sizeof(usize)
    );
    *cursor = 0U;
    cursor += this->layer_capacity;
  }
  this->get_image_id_ptr()[index] = layer_id;

  std::memmove(
      this->layer_info + index + 1U, this->layer_info + index,
      sizeof(LayerInfo) * (this->layer_count - index) // NOLINT
  );
  std::strcpy(
      this->layer_info[index].name, get_default_name(this->layer_count + 1)
  );
  this->layer_info[index].opacity = VISIBLE_BIT | 100U;

  ++this->layer_count;

  return error_code::OK;
}

bool TimelineInfo::toggle_layer_visibility(usize index) noexcept {
  assert(index >= 0 && index < this->layer_count);

  auto& info = this->layer_info[index].opacity;
  info ^= VISIBLE_BIT;
  return info & VISIBLE_BIT;
}

// === Memory === //

error_code TimelineInfo::allocate_by_capacities(
    usize layer_capacity, usize frame_capacity
) noexcept {
  TRY(this->allocate_timeline_capacities(layer_capacity, frame_capacity));
  TRY(this->allocate_layer_info_capacity(layer_capacity));
  return error_code::OK;
}

error_code TimelineInfo::allocate_timeline(usize new_size) noexcept {
  this->timeline_alloc_size = new_size;
  // NOLINTNEXTLINE
  this->timeline = (usize*)std::malloc(this->timeline_alloc_size);
  return this->timeline == nullptr ? error_code::BAD_ALLOC : error_code::OK;
}

error_code TimelineInfo::allocate_timeline_capacities(
    usize layer_capacity, usize frame_capacity
) noexcept {
  // +1 is the frame id
  return this->allocate_timeline(
      // NOLINTNEXTLINE
      (layer_capacity + 1U) * frame_capacity * sizeof(usize)
  );
}

error_code TimelineInfo::reallocate_timeline_on_layer(usize new_layer_capacity
) noexcept {
  auto new_capacity =
      // NOLINTNEXTLINE
      (new_layer_capacity + 1) * this->frame_capacity * sizeof(usize);
  auto* new_ptr = (usize*)std::realloc(this->timeline, new_capacity); // NOLINT
  if (new_ptr == nullptr) {
    return error_code::BAD_ALLOC;
  }

  this->timeline = new_ptr;

  auto* dst_cursor =
      // NOLINTNEXTLINE
      this->get_image_id_ptr() + (this->frame_count - 1U) * new_layer_capacity;
  auto* src_cursor = this->get_image_id_ptr(this->frame_count - 1U);
  for (usize i = 1U; i < this->frame_count; ++i) {
    std::memmove(dst_cursor, src_cursor, this->layer_count * sizeof(usize));
    dst_cursor -= new_layer_capacity;
    src_cursor -= this->layer_capacity;
  }

  this->layer_capacity = new_layer_capacity;
  this->timeline_alloc_size = new_capacity;

  return error_code::OK;
}

error_code TimelineInfo::allocate_layer_info(usize new_size) noexcept {
  this->layer_info_alloc_size = new_size;
  // NOLINTNEXTLINE
  this->layer_info = (LayerInfo*)malloc(new_size);
  return this->layer_info == nullptr ? error_code::BAD_ALLOC : error_code::OK;
}

error_code TimelineInfo::allocate_layer_info_capacity(usize layer_capacity
) noexcept {
  return this->allocate_layer_info(layer_capacity * sizeof(LayerInfo));
}

error_code TimelineInfo::reallocate_layer_info(usize new_size) noexcept {
  // NOLINTNEXTLINE
  auto* new_ptr =
      // NOLINTNEXTLINE
      (LayerInfo*)std::realloc(this->layer_info, new_size);
  if (new_ptr == nullptr) {
    return error_code::BAD_ALLOC;
  }
  this->layer_info = new_ptr;
  return error_code::OK;
}

// === Private Accessors === //

usize* TimelineInfo::get_frame_id_ptr() const noexcept {
  return this->timeline;
}

usize TimelineInfo::get_frame_id(u32 index) const noexcept {
  return this->timeline[index];
}

usize* TimelineInfo::get_image_id_ptr(usize frame_index) const noexcept {
  // NOLINTNEXTLINE
  return this->timeline + this->frame_capacity +
         this->layer_capacity * frame_index;
}

// === Iterators === //

FrameIter::FrameIter(
    usize* ptr, usize frame_count, usize frame_capacity, usize layer_capacity
) noexcept
    : id_ptr(ptr),
      frame_ptr(ptr + frame_capacity),
      size(frame_count),
      capacity(layer_capacity) {}

FrameIter& FrameIter::operator++() noexcept {
  assert(this->id_ptr != nullptr);

  --this->size;
  if (this->size == 0) {
    id_ptr = nullptr;
    return *this;
  }

  ++id_ptr;
  frame_ptr += this->capacity;

  return *this;
}

usize FrameIter::get_id() const noexcept {
  return *this->id_ptr;
}

usize FrameIter::get_image_id(usize layer_index) const noexcept {
  return this->frame_ptr[layer_index];
}

FrameIter::operator bool() const noexcept {
  return this->id_ptr != nullptr;
}

#ifndef NDEBUG

void TimelineInfo::print_metadata() const noexcept {
  logger::debug(
      "Timeline Info Metadata\n"
      "  Layer Count/Cap: %u/%u\n"
      "  Frame Count/Cap: %u/%u\n"
      "  Timeline Alloc Size: %u\n"
      "  Layer Info Alloc Size: %u",
      this->layer_count, this->layer_capacity, this->frame_count,
      this->frame_capacity, this->timeline_alloc_size,
      this->layer_info_alloc_size
  );
}

#endif

} // namespace draw

