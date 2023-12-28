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

class FrameIter {
public:
  FrameIter() noexcept;
  FrameIter(
      usize* ptr, usize frame_index, usize frame_count, usize frame_capacity,
      usize layer_capacity
  ) noexcept;

  FrameIter& operator++() noexcept;
  FrameIter& operator--() noexcept;

  [[nodiscard]] usize get_index() const noexcept;
  [[nodiscard]] usize get_image_id(usize layer_index) const noexcept;

  explicit operator bool() const noexcept;

private:
  usize* ptr;
  usize index;
  usize size;
  usize next_frame;
};

// SOA Timeline / AOS Images Info
class TimelineInfo {
public:
  TimelineInfo() noexcept;
  TimelineInfo(const TimelineInfo&) noexcept = delete;
  TimelineInfo& operator=(const TimelineInfo&) noexcept = delete;

  [[nodiscard]] error_code init() noexcept;
  [[nodiscard]] error_code
  load_init(usize layer_count, usize frame_count) noexcept;
  void load_layer(usize index, LayerInfo layer_info) noexcept;
  void load_frame(usize index, usize* image_ids) noexcept;

  TimelineInfo(TimelineInfo&& rhs) noexcept;
  TimelineInfo& operator=(TimelineInfo&& rhs) noexcept;
  [[nodiscard]] error_code copy(const TimelineInfo& other) noexcept;
  ~TimelineInfo() noexcept;

  // === Accessors === //

  [[nodiscard]] usize
  get_image_id(usize frame_index, usize layer_index) const noexcept;

  [[nodiscard]] usize get_layer_count() const noexcept;
  [[nodiscard]] usize get_frame_count() const noexcept;
  [[nodiscard]] usize get_layer_capacity() const noexcept;

  [[nodiscard]] const LayerInfo& get_layer_info(usize index) const noexcept;
  [[nodiscard]] const c8* get_layer_name(usize index) const noexcept;
  [[nodiscard]] bool is_layer_visible(usize index) const noexcept;

  [[nodiscard]] Frame get_frame(usize index) const noexcept;

  [[nodiscard]] usize get_timeline_alloc_size() const noexcept;

  [[nodiscard]] FrameIter get_frame_iter(usize frame_index = 0U) const noexcept;

  // === Modifiers === //

  [[nodiscard]] error_code insert_layer(usize layer_index) noexcept;
  [[nodiscard]] error_code insert_frame(usize frame_index) noexcept;
  void
  set_image_id(usize frame_index, usize layer_index, usize img_id) noexcept;

  void remove_layer(usize index) noexcept;
  void remove_frame(usize index) noexcept;
  void remove_image(usize frame_index, usize layer_index) noexcept;

  void set_layer_info(usize index, const LayerInfo& info) noexcept;
  bool toggle_layer_visibility(usize index) noexcept;
  void set_layer_visibility(usize index, bool visibility) noexcept;

#ifndef NDEBUG

  void print_metadata() const noexcept;

#endif

private:
  usize* timeline;
  LayerInfo* layer_info;
  usize layer_count;
  usize frame_count;
  usize layer_capacity;
  usize frame_capacity;
  usize timeline_alloc_size;
  usize layer_info_alloc_size;

  // === Copy Helpers === //
  [[nodiscard]] error_code copy_empty(const TimelineInfo& other) noexcept;
  void copy_normal(const TimelineInfo& other) noexcept;
  void copy_overfit(const TimelineInfo& other) noexcept;
  [[nodiscard]] error_code copy_grow(const TimelineInfo& other) noexcept;

  // === Memory === //
  [[nodiscard]] error_code
  allocate_by_capacities(usize layer_capacity, usize frame_capacity) noexcept;

  [[nodiscard]] error_code allocate_timeline(usize new_size) noexcept;
  [[nodiscard]] error_code allocate_timeline_capacities(
      usize layer_capacity, usize frame_capacity
  ) noexcept;
  [[nodiscard]] error_code reallocate_timeline_on_layer(usize new_layer_capacity
  ) noexcept;
  [[nodiscard]] error_code reallocate_timeline_on_frame(usize new_frame_capacity
  ) noexcept;

  [[nodiscard]] error_code allocate_layer_info(usize layer_capacity) noexcept;
  [[nodiscard]] error_code reallocate_layer_info(usize new_layer_capacity
  ) noexcept;

  // === Private Accessors === //

  [[nodiscard]] inline usize get_frame_index(u32 index) const noexcept;
  [[nodiscard]] inline usize* get_image_id_ptr(
      usize frame_index = 0U, usize layer_index = 0U
  ) const noexcept;
};

} // namespace draw

#endif

