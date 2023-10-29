/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-26
 *==========================*/

#ifndef PXL_VIEW_SDL3_WIDGET_MENU_BTN_HPP
#define PXL_VIEW_SDL3_WIDGET_MENU_BTN_HPP

#include "../texture.hpp"
#include "./widget.hpp"

namespace view::sdl3::widget {

class MenuBtn final : public Widget {
public:
  MenuBtn() noexcept = default;
  MenuBtn(const MenuBtn&) noexcept = delete;
  MenuBtn& operator=(const MenuBtn&) noexcept = delete;
  MenuBtn(MenuBtn&&) noexcept = default;
  MenuBtn& operator=(MenuBtn&&) noexcept = default;
  ~MenuBtn() noexcept override = default;

  void set_texture(Texture&& tex) noexcept;
  void set_left_click_listener(void (*left_click_listener)()) noexcept;

  void reset() noexcept override;
  void input(const event::Input& evt, Data& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

private:
  Texture tex{};

public:
  frect tex_rect{};

private:
  // NOLINTNEXTLINE
  input::BtnState state = input::BtnState::NORMAL;
  bool highlight = false;
  /* bool focus = false; */

  void (*left_click_listener)() = nullptr;
};

} // namespace view::sdl3::widget

#endif

