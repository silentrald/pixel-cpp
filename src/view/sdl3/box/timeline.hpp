/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#ifndef PXL_VIEW_SDL3_TIMELINE_BOX_HPP
#define PXL_VIEW_SDL3_TIMELINE_BOX_HPP

#include "../texture.hpp"
#include "../widget/button.hpp"
#include "../widget/textbox.hpp"
#include "./box.hpp"
#include "core/draw/anim.hpp"
#include "core/ds/vector.hpp"

namespace view::sdl3::widget {

class TimelineBox final : public Box {
public:
  TimelineBox() noexcept = default;
  TimelineBox(const TimelineBox&) noexcept = delete;
  TimelineBox& operator=(const TimelineBox&) noexcept = delete;
  TimelineBox(TimelineBox&&) noexcept = default;
  TimelineBox& operator=(TimelineBox&&) noexcept = default;
  ~TimelineBox() noexcept override = default;

  void init(const Renderer& renderer) noexcept;

  [[nodiscard]] error_code insert_layer_info(
      i32 index, const draw::LayerInfo& layer_info, const Renderer& renderer
  ) noexcept;
  void set_layer_visible(i32 index, bool visible) noexcept;
  void clear_layers() noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void locale_updated(const Renderer& renderer) noexcept override;
  void input(const event::Input& evt, Data& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

  u32 start_frame = 1U;
  u32 end_frame = 1U;
  i32 selected_layer = 0;
  u32 selected_frame = 1U;

private:
  struct Layer {
    Textbox textbox{};
    bool visible = true;
  };

  widget::Button add_btn{};
  ds::vector<Layer> layers{};

  void render_grid(const Renderer& renderer) const noexcept;
  void render_frames(const Renderer& renderer) const noexcept;
  void render_layers(const Renderer& renderer) const noexcept;
};

} // namespace view::sdl3::widget

#endif

