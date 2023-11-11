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
  c8 name[51] = ""; // NOLINT
  u8 opacity = 0U;
};

class FrameIter {
public:
  FrameIter() noexcept = default;
  FrameIter(
      usize* ptr, usize frame_count, usize frame_capacity, usize layer_capacity
  ) noexcept;

  FrameIter& operator++() noexcept;

  [[nodiscard]] usize get_id() const noexcept;
  [[nodiscard]] usize get_image_id(usize layer_index) const noexcept;

  explicit operator bool() const noexcept;

private:
  usize* id_ptr = nullptr;
  usize* frame_ptr = nullptr;
  usize size = 0U; // Use this as a index counter
  usize capacity = 0U;
};

// SOA Timeline / AOS Images Info
class TimelineInfo {
public:
  TimelineInfo() noexcept = default;
  TimelineInfo(const TimelineInfo&) noexcept = delete;
  TimelineInfo& operator=(const TimelineInfo&) noexcept = delete;

  void init() noexcept;
  void load_init(usize layer_count, usize frame_count) noexcept;
  void load_layer(usize index, LayerInfo layer_info) noexcept;
  void load_frame(usize index, usize id, usize* image_ids) noexcept;

  TimelineInfo(TimelineInfo&& rhs) noexcept;
  TimelineInfo& operator=(TimelineInfo&& rhs) noexcept;
  void copy(const TimelineInfo& other) noexcept;
  /* void minicopy(const TimelineInfo& other) noexcept; */
  ~TimelineInfo() noexcept;

  [[nodiscard]] usize
  get_image_id(usize frame_id, usize layer_index) const noexcept;

  [[nodiscard]] usize get_layer_count() const noexcept;
  [[nodiscard]] usize get_frame_count() const noexcept;
  [[nodiscard]] usize get_layer_capacity() const noexcept;

  [[nodiscard]] const LayerInfo& get_layer_info(usize index) const noexcept;
  [[nodiscard]] const c8* get_layer_name(usize index) const noexcept;
  [[nodiscard]] bool is_layer_visible(usize index) const noexcept;

  [[nodiscard]] Frame get_frame(usize id) const noexcept;

  [[nodiscard]] usize get_timeline_alloc_size() const noexcept;

  [[nodiscard]] FrameIter get_frame_iter() const noexcept;

  void insert_layer(usize index, usize layer_id) noexcept;
  bool toggle_layer_visibility(usize index) noexcept;

private:
  usize* timeline = nullptr;
  LayerInfo* layer_info = nullptr;
  usize layer_count = 0;
  usize frame_count = 0;
  usize layer_capacity = 0;
  usize frame_capacity = 0;
  usize timeline_alloc_size = 0;
  usize layer_info_alloc_size = 0;

  // === Copy Helpers === //
  void copy_empty(const TimelineInfo& other) noexcept;
  void copy_normal(const TimelineInfo& other) noexcept;
  void copy_overfit(const TimelineInfo& other) noexcept;
  void copy_grow(const TimelineInfo& other) noexcept;

  // === Memory === //
  void
  allocate_by_capacities(usize layer_capacity, usize frame_capacity) noexcept;

  void allocate_timeline(usize new_size) noexcept;
  void allocate_timeline_capacities(
      usize layer_capacity, usize frame_capacity
  ) noexcept;
  void reallocate_timeline_on_layer(usize new_layer_capacity) noexcept;

  void allocate_layer_info(usize new_size) noexcept;
  void allocate_layer_info_capacity(usize layer_capacity) noexcept;
  void reallocate_layer_info(usize new_size) noexcept;

  // === Private Accessors === //

  [[nodiscard]] inline usize* get_frame_id_ptr() const noexcept;
  [[nodiscard]] inline usize get_frame_id(u32 index) const noexcept;
  [[nodiscard]] inline usize*
  get_image_id_ptr(usize frame_index = 0U) const noexcept;
};

} // namespace draw

#endif

