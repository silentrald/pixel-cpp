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

  void init(const Renderer& renderer) noexcept;

  [[nodiscard]] error_code insert_layer_info(
      usize index, const draw::LayerInfo& layer_info, const Renderer& renderer
  ) noexcept;
  void set_layer_visible(usize index, bool visible) noexcept;
  void clear_layers() noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void locale_updated(const Renderer& renderer) noexcept override;
  void input(const event::Input& evt, InputData& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

  usize start_frame;
  usize end_frame;
  // Currently shown image on the canvas
  usize active_layer;
  usize active_frame;
  // NOTE: Selected layer/frame maybe use a vector for this one
  usize selected_layer;
  /* usize selected_frame; */

private:
  struct Layer {
    Textbox textbox{};
    bool visible = true;
  };

  ds::vector<Layer> layers{};

  void handle_mouse_left(const event::Input& evt, InputData& data) noexcept;
  void handle_mouse_right(const event::Input& evt, InputData& data) noexcept;

  void render_grid(const Renderer& renderer) const noexcept;
  void render_frames(const Renderer& renderer) const noexcept;
  void render_layers(const Renderer& renderer) const noexcept;
};

} // namespace view::sdl3::widget

#endif

