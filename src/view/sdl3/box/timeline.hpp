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

  [[nodiscard]] error_code init() noexcept;

  [[nodiscard]] error_code
  insert_layer(usize index, const draw::LayerInfo& layer_info) noexcept;
  void remove_layer(usize index) noexcept;
  void set_layer_visible(usize index, bool visible) noexcept;
  void set_anim(const draw::Anim* anim) noexcept;
  void clear_layers() noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void locale_updated() noexcept override;
  void input(const event::Input& evt, InputData& data) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

  usize start_frame;
  usize end_frame;
  // Currently shown image on the canvas
  usize active_layer;
  usize active_frame;
  // NOTE: Selected layer/frame maybe use a vector for this one
  usize selected_layer;
  usize selected_frame;

private:
  const draw::Anim* anim = nullptr;
  ds::vector<frect> rects{};
  struct Layer {
    Textbox textbox{};
    bool visible = true;
  };

  ds::vector<Layer> layers{};

  void handle_mouse_left(const event::Input& evt, InputData& data) noexcept;
  void handle_mouse_right(const event::Input& evt, InputData& data) noexcept;

  void render_grid() const noexcept;
  void render_frame_numbers() const noexcept;
  void render_frames() noexcept;
  void render_layers() noexcept;
};

} // namespace view::sdl3::widget

#endif

