/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-25
 *==========================*/

#ifndef PXL_VIEW_SDL3_MENU_BOX_HPP
#define PXL_VIEW_SDL3_MENU_BOX_HPP

#include "../widget/menu_btn.hpp"
#include "./box.hpp"
#include "core/ds/vector.hpp"

namespace view::sdl3::widget {

class MenuBox final : public Box {
public:
  MenuBox() noexcept = default;
  MenuBox(const MenuBox&) noexcept = delete;
  MenuBox& operator=(const MenuBox&) noexcept = delete;
  MenuBox(MenuBox&&) noexcept = default;
  ~MenuBox() noexcept override = default;
  MenuBox& operator=(MenuBox&&) noexcept = default;

  void push_menu_btn(MenuBtn&& btn) noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void input(const event::Input& evt, Data& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

  ds::vector<MenuBtn> btns{};
};

} // namespace view::sdl3::widget

#endif

