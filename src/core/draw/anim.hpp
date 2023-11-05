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
#include "./image_db.hpp"
#include "./timeline_info.hpp"
#include "./types.hpp"
#include "core/ds/vector.hpp"
#include "types.hpp"

namespace draw {

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

  [[nodiscard]] const ImageDb& get_image_db() const noexcept;
  [[nodiscard]] const TimelineInfo& get_timeline_info() const noexcept;

  [[nodiscard]] bool has_point(ivec pos) const noexcept;

  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;

  [[nodiscard]] i32 get_layer_count() const noexcept;
  [[nodiscard]] i32 get_frame_count() const noexcept;

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

