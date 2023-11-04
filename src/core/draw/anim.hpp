/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-29
 *==========================*/

#ifndef PXL_DRAW_ANIM_HPP
#define PXL_DRAW_ANIM_HPP

#include "./frame.hpp"
#include "./image.hpp"
#include "./types.hpp"
#include "core/ds/vector.hpp"
#include "types.hpp"

namespace draw {

// SOA Images Db
class ImageDb {
public:
  ImageDb() noexcept = default;
  ImageDb(const ImageDb&) noexcept = delete;
  ImageDb& operator=(const ImageDb&) noexcept = delete;

  void init(i32 bytes, i32 capacity) noexcept;
  ImageDb(ImageDb&& rhs) noexcept;
  ImageDb& operator=(ImageDb&& rhs) noexcept;
  void copy(const ImageDb& other) noexcept;
  void minicopy(const ImageDb& other) noexcept;
  ~ImageDb() noexcept;

  [[nodiscard]] data_ptr get_ptr() const noexcept;
  [[nodiscard]] data_ptr get_image(u32 id) const noexcept;

  void create_layer(u32 new_id) noexcept;

private:
  // TODO: Change datatypes (64 bit) to support higher sizes
  data_ptr ptr = nullptr;
  i32 insert = 0;
  i32 last = 0;
  i32 capacity = 0;
  i32 size = 0;

  i32 bytes = 0;
  i32 alloc_size = 0;

  // === Copy Helpers === //
  void copy_empty(const ImageDb& other) noexcept;
  void copy_normal(const ImageDb& other) noexcept;
  void copy_overfit(const ImageDb& other) noexcept;
  void copy_grow(const ImageDb& other) noexcept;

  // === Memory === //
  void allocate(i32 timeline_alloc_size) noexcept;
  void reallocate(i32 new_timeline_alloc_size) noexcept;
};

struct LayerInfo {
  c8 name[51]{}; // NOLINT
  u8 opacity = 0U;
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
  [[nodiscard]] i32 get_layer_capacity() const noexcept;
  [[nodiscard]] const LayerInfo& get_layer_info(i32 index) const noexcept;
  [[nodiscard]] const c8* get_layer_name(i32 index) const noexcept;
  [[nodiscard]] bool is_layer_visible(i32 index) const noexcept;

  [[nodiscard]] Frame get_frame(u32 id) const noexcept;

  [[nodiscard]] i32 get_timeline_alloc_size() const noexcept;

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

/**
 * Animation data containing the image information for frames/layers
 **/
class Anim {
public:
  Anim() noexcept = default;
  Anim(const Anim&) noexcept = delete;
  Anim& operator=(const Anim&) noexcept = delete;
  Anim(Anim&& rhs) noexcept = default;
  Anim& operator=(Anim&& rhs) noexcept = default;
  ~Anim() noexcept = default;

  void init(ivec size, ColorType type) noexcept;
  /* void load(const c8* str) noexcept; // For file loading */

  void copy(const Anim& other) noexcept;
  void minicopy(const Anim& other) noexcept;
  void clear() noexcept;

  [[nodiscard]] bool has_point(ivec pos) const noexcept;

  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;

  [[nodiscard]] i32 get_layer_count() const noexcept;

  [[nodiscard]] Image get_image(u32 id) const noexcept;
  [[nodiscard]] Image get_image(u32 frame_id, i32 layer_index) const noexcept;
  [[nodiscard]] u32 get_image_id(u32 frame_id, i32 layer_index) const noexcept;

  [[nodiscard]] const LayerInfo& get_layer_info(i32 index) const noexcept;
  [[nodiscard]] const c8* get_layer_name(i32 index) const noexcept;
  [[nodiscard]] bool is_layer_visible(i32 index) const noexcept;
  void get_flatten(
      u32 frame_id, i32 start_layer, i32 end_layer, ds::vector<rgba8>& pixels
  ) const noexcept;

  /**
   * @param index - where to insert the new layer
   * @returns id of the newly created image on the layer
   **/
  u32 insert_layer(i32 index) noexcept;
  bool toggle_layer_visibility(i32 index) noexcept;

  [[nodiscard]] explicit operator bool() const noexcept;

private:
  ImageDb images{};
  TimelineInfo timeline{};

  u32 images_id = 0U;

  // Dimension size of the animation
  ivec size{};

  // What color depth is being used
  ColorType type = ColorType::NONE;
};

} // namespace draw

#endif

