/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#ifndef PXL_VIEW_SDL3_TIMELINE_BOX_HPP
#define PXL_VIEW_SDL3_TIMELINE_BOX_HPP

#include "../input/button.hpp"
#include "../input/textbox.hpp"
#include "../texture.hpp"
#include "../widget/horizontal_scrollbar.hpp"
#include "../widget/vertical_scrollbar.hpp"
#include "./box.hpp"
#include "core/draw/anim.hpp"
#include "core/ds/vector.hpp"

namespace view::sdl3::widget {

class TimelineBox final : public Box {
public:
  TimelineBox() noexcept;
  TimelineBox(const TimelineBox&) noexcept = delete;
  TimelineBox& operator=(const TimelineBox&) noexcept = delete;
  TimelineBox(TimelineBox&&) noexcept = default;
  TimelineBox& operator=(TimelineBox&&) noexcept = default;
  ~TimelineBox() noexcept override = default;

  void init() noexcept;

  // === Setters === //

  void set_layer_offset(usize layer_offset) noexcept;
  void set_frame_offset(usize frame_offset) noexcept;
  void set_active_image(usize frame_index, usize layer_index) noexcept;
  [[nodiscard]] error_code set_anim(const draw::Anim* anim) noexcept;

  // === Modifiers === //

  [[nodiscard]] error_code
  insert_layer(usize index, const draw::LayerInfo& layer_info) noexcept;
  void remove_layer(usize index) noexcept;
  void clear_layers() noexcept;

  void set_layer_visible(usize index, bool visible) noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void update_locale() noexcept override;
  void input(const event::Input& evt) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

  // NOTE: Selected layer/frame maybe use a vector for this one
  usize selected_layer;
  usize selected_frame;

private:
  // starting layer to show on the view
  usize layer_offset;
  // maximum number of layers to show on screen
  usize max_layers;
  // starting frame to show on the view
  usize frame_offset;
  // maximum number of frames to show on screen
  usize max_frames;

  // Currently shown image on the canvas
  usize active_layer;
  // Currently shown image on the canvas
  usize active_frame;

  const draw::Anim* anim = nullptr;
  ds::vector<frect> rects{};

  struct Layer {
    Textbox textbox{};
    bool visible = true;
  };
  ds::vector<Layer> layers{};

  VerticalScrollbar vertical_scrollbar{};
  HorizontalScrollbar horizontal_scrollbar{};

  void adjust_layer_textboxes() noexcept;

  void handle_mouse_left(const event::Input& evt) noexcept;
  void handle_mouse_right(const event::Input& evt) noexcept;

  void render_grid() const noexcept;
  void render_frame_numbers() const noexcept;
  void render_frames() noexcept;
  void render_layers() noexcept;
};

} // namespace view::sdl3::widget

#endif

