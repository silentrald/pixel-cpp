/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-05
 *==========================*/

#ifndef PXL_DRAW_TIMELINE_INFO_HPP
#define PXL_DRAW_TIMELINE_INFO_HPP

#include "./frame.hpp"
#include "./types.hpp"
#include "types.hpp"

namespace draw {

struct LayerInfo {
  c8 name[51]{}; // NOLINT
  u8 opacity = 0U;
};

class FrameIter {
public:
  FrameIter() noexcept = default;
  FrameIter(
      u32* ptr, i32 frame_count, i32 frame_capacity, i32 layer_capacity
  ) noexcept;

  FrameIter& operator++() noexcept;

  [[nodiscard]] u32 get_id() const noexcept;
  [[nodiscard]] u32 get_image_id(i32 layer_index) const noexcept;

  explicit operator bool() const noexcept;

private:
  u32* id_ptr = nullptr;
  u32* frame_ptr = nullptr;
  i32 size = 0; // Use this as a index counter
  i32 capacity = 0;
};

// SOA Timeline / AOS Images Info
class TimelineInfo {
public:
  TimelineInfo() noexcept = default;
  TimelineInfo(const TimelineInfo&) noexcept = delete;
  TimelineInfo& operator=(const TimelineInfo&) noexcept = delete;

  void init() noexcept;
  TimelineInfo(TimelineInfo&& rhs) noexcept;
  TimelineInfo& operator=(TimelineInfo&& rhs) noexcept;
  void copy(const TimelineInfo& other) noexcept;
  /* void minicopy(const TimelineInfo& other) noexcept; */
  ~TimelineInfo() noexcept;

  [[nodiscard]] u32 get_image_id(i32 frame_id, i32 layer_index) const noexcept;

  [[nodiscard]] i32 get_layer_count() const noexcept;
  [[nodiscard]] i32 get_frame_count() const noexcept;
  [[nodiscard]] i32 get_layer_capacity() const noexcept;

  [[nodiscard]] const LayerInfo& get_layer_info(i32 index) const noexcept;
  [[nodiscard]] const c8* get_layer_name(i32 index) const noexcept;
  [[nodiscard]] bool is_layer_visible(i32 index) const noexcept;

  [[nodiscard]] Frame get_frame(u32 id) const noexcept;

  [[nodiscard]] i32 get_timeline_alloc_size() const noexcept;

  [[nodiscard]] FrameIter get_frame_iter() const noexcept;

  void insert_layer(i32 index, u32 layer_id) noexcept;
  bool toggle_layer_visibility(i32 index) noexcept;

private:
  u32* timeline = nullptr;
  LayerInfo* layer_info = nullptr;
  i32 layer_count = 0;
  i32 frame_count = 0;
  i32 layer_capacity = 0;
  i32 frame_capacity = 0;
  i32 timeline_alloc_size = 0;
  i32 layer_info_alloc_size = 0;

  // === Copy Helpers === //
  void copy_empty(const TimelineInfo& other) noexcept;
  void copy_normal(const TimelineInfo& other) noexcept;
  void copy_overfit(const TimelineInfo& other) noexcept;
  void copy_grow(const TimelineInfo& other) noexcept;

  // === Memory === //
  void allocate_timeline(i32 new_size) noexcept;
  void reallocate_timeline_on_layer(i32 new_layer_capacity) noexcept;

  void allocate_layer_info(i32 new_size) noexcept;
  void reallocate_layer_info(i32 new_size) noexcept;
};

} // namespace draw

#endif

