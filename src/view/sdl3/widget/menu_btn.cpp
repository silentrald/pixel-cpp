/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-26
 *==========================*/

#include "./menu_btn.hpp"
#include "core/cfg/theme.hpp"

namespace view::sdl3::widget {

void MenuBtn::set_texture(Texture&& tex) noexcept {
  this->tex = std::move(tex);
}

void MenuBtn::set_left_click_listener(void (*left_click_listener)()) noexcept {
  this->left_click_listener = left_click_listener;
}

void MenuBtn::reset() noexcept {
  if (this->state == input::BtnState::DISABLED) {
    return;
  }

  this->state = input::BtnState::NORMAL;
}

void MenuBtn::input(const event::Input& evt) noexcept {
  if (this->state == input::BtnState::DISABLED) {
    return;
  }

  if (!this->rect.has_point(evt.mouse.pos)) {
    this->state = input::BtnState::NORMAL;
    return;
  }

  switch (evt.mouse.left.state) {
  case input::MouseState::DOWN:
  case input::MouseState::HOLD:
    this->state = input::BtnState::DOWN;
    break;

  case input::MouseState::UP:
    this->state = input::BtnState::DOWN;
    if (this->left_click_listener) {
      this->left_click_listener();
    }
    break;

  case input::MouseState::NONE:
    this->state = input::BtnState::HOVER;
    break;
  }
}

void MenuBtn::update() noexcept {
  // Do nothing UwU
}

void MenuBtn::render(const Renderer& renderer) const noexcept {
  // Draw bg
  renderer.set_color(cfg::theme::get_menu_btn_color(this->state));
  renderer.fill_rect(this->rect);

  // Draw tex
  renderer.render_texture(this->tex, this->tex_rect);
}

} // namespace view::sdl3::widget

