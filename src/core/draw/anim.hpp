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
#include <string>

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

  [[nodiscard]] error_code init(ivec size, ColorType type) noexcept;

  [[nodiscard]] error_code load_init(
      ivec size, usize image_count, usize layer_count, usize frame_count
  ) noexcept;
  void load_layer(usize index, LayerInfo layer_info) noexcept;
  void load_frame(usize index, usize* image_ids) noexcept;
  [[nodiscard]] error_code
  load_image(usize index, usize id, data_ptr pixels) noexcept;
  [[nodiscard]] error_code load_finish() noexcept;

  [[nodiscard]] error_code copy(const Anim& other) noexcept;
  [[nodiscard]] error_code minicopy(const Anim& other) noexcept;
  void clear() noexcept;

  [[nodiscard]] const ImageDb& get_image_db() const noexcept;
  [[nodiscard]] const TimelineInfo& get_timeline_info() const noexcept;

  [[nodiscard]] bool has_point(ivec pos) const noexcept;

  // === Accessors === //

  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;

  [[nodiscard]] usize get_layer_count() const noexcept;
  [[nodiscard]] usize get_frame_count() const noexcept;

  [[nodiscard]] Frame get_frame(usize index) const noexcept;

  [[nodiscard]] expected<Image> get_image(usize id) noexcept;
  [[nodiscard]] expected<Image>
  get_image(usize frame_index, usize layer_index) noexcept;
  [[nodiscard]] Image get_image_fast(usize id) const noexcept;
  [[nodiscard]] Image
  get_image_fast(usize frame_index, usize layer_index) const noexcept;
  [[nodiscard]] usize get_image_bytes_size() const noexcept;

  [[nodiscard]] expected<data_ptr> get_pixels(usize id) noexcept;
  [[nodiscard]] expected<data_ptr>
  get_pixels(usize frame_index, usize layer_index) noexcept;
  [[nodiscard]] data_ptr get_pixels_fast(usize id) const noexcept;
  [[nodiscard]] data_ptr
  get_pixels_fast(usize frame_index, usize layer_index) const noexcept;
  [[nodiscard]] error_code
  get_pixels_slow(usize id, data_ptr pixels) const noexcept;
  [[nodiscard]] error_code get_pixels_slow(
      usize frame_index, usize layer_index, data_ptr pixels
  ) const noexcept;

  [[nodiscard]] usize
  get_image_id(usize frame_index, usize layer_index) const noexcept;

  [[nodiscard]] const LayerInfo& get_layer_info(usize index) const noexcept;
  [[nodiscard]] const c8* get_layer_name(usize index) const noexcept;
  [[nodiscard]] bool is_layer_visible(usize index) const noexcept;
  void get_flatten(
      usize frame_index, usize start_layer, usize end_layer,
      ds::vector<data_type>& pixels
  ) const noexcept;

  const std::string& get_name() noexcept;

  // === Iterators === //

  [[nodiscard]] FrameIter get_frame_iter(usize frame_index = 0U) const noexcept;

  // === Modifiers === //

  /**
   * @param index - where to insert the new layer
   **/
  [[nodiscard]] error_code insert_layer(usize index) noexcept;

  [[nodiscard]] error_code insert_frame(usize frame_index) noexcept;

  /**
   * Adds an image to the timeline position. The timeline value at that specific
   *  position must be 0.
   * @returns id of the image created
   **/
  [[nodiscard]] expected<usize>
  create_image(usize frame_index, usize layer_index) noexcept;

  [[nodiscard]] error_code remove_layer(usize index) noexcept;
  [[nodiscard]] error_code remove_frame(usize index) noexcept;
  [[nodiscard]] error_code
  remove_image(usize frame_index, usize layer_index) noexcept;

  void set_layer_info(usize index, const LayerInfo& info) noexcept;
  bool toggle_layer_visibility(usize index) noexcept;
  void set_layer_visibility(usize index, bool visibility) noexcept;

  [[nodiscard]] error_code write_pixels_to_disk(usize id) const noexcept;
  [[nodiscard]] error_code write_image_to_disk(const Image& image
  ) const noexcept;

  [[nodiscard]] explicit operator bool() const noexcept;

  // === Debugging === //

#ifndef NDEBUG

  void print_metadata(bool lock = true) const noexcept;
  void print_timeline_info_metadata(bool lock = true) const noexcept;
  void print_images_memory(bool lock = true) const noexcept;
  void print_images_disk(bool lock = true) const noexcept;
  void print_timeline_info(bool lock = true) const noexcept;

#endif

private:
  ImageDb images{};
  TimelineInfo timeline{};

  // Dimension size of the animation
  ivec size{};

  // What color depth is being used
  ColorType type = ColorType::NONE;

  usize frame_end = 0U;

  /* std::string filepath{}; */
  std::string name{};
};

} // namespace draw

#endif

