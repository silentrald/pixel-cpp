/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-23
 *==========================*/

#ifndef PXL_VIEW_SDL3_TOOL_BOX_HPP
#define PXL_VIEW_SDL3_TOOL_BOX_HPP

#include "../widget/button.hpp"
#include "./box.hpp"
#include "core/ds/vector.hpp"

namespace view::sdl3::widget {

class ToolBox final : public Box {
public:
  ToolBox() noexcept = default;
  ToolBox(const ToolBox&) noexcept = delete;
  ToolBox& operator=(const ToolBox&) noexcept = delete;
  ToolBox(ToolBox&&) noexcept = default;
  ToolBox& operator=(ToolBox&&) noexcept = default;
  ~ToolBox() noexcept override = default;

  void push_btn(Button&& btn) noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void input(const event::Input& evt) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

  ds::vector<Button> btns{};
};

} // namespace view::sdl3::widget

#endif

