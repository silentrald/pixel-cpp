/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-22
 *==========================*/

#include "./button.hpp"
#include "core/cfg/theme.hpp"

namespace view::sdl3::widget {

bool Button::is_enabled() const noexcept {
  return (this->info & input::BtnMask::STATES) != input::BtnState::DISABLED;
}

void Button::set_theme(input::BtnTheme theme) noexcept {
  this->info = (this->info & ~input::BtnMask::THEME) | theme;
}

void Button::set_texture(Texture&& tex) noexcept {
  this->tex = std::move(tex);
}

void Button::set_left_click_listener(void (*left_click_listener)()) noexcept {
  this->left_click_listener = left_click_listener;
}

void Button::set_right_click_listener(void (*right_click_listener)()) noexcept {
  this->right_click_listener = right_click_listener;
}

void Button::enable() noexcept {
  this->info = (this->info & ~input::BtnMask::STATES) | input::BtnState::NORMAL;
}

void Button::disable() noexcept {
  this->info =
      (this->info & ~input::BtnMask::STATES) | input::BtnState::DISABLED;
}

void Button::reset() noexcept {
  this->focused = false;

  if ((this->info & input::BtnMask::STATES) == input::BtnState::DISABLED) {
    return;
  }

  this->info = (this->info & ~input::BtnMask::STATES) | input::BtnState::NORMAL;
}

void Button::unfocused() noexcept {
  this->focused = false;
}

void Button::locale_updated() noexcept {
  // Do nothing UwU
}

void Button::input(const event::Input& evt, InputData& data) noexcept {
  if ((this->info & input::BtnMask::STATES) == input::BtnState::DISABLED) {
    return;
  }

  // Check if the mouse in the bounding box of the rect
  if (!this->rect.has_point(evt.mouse.pos)) {
    this->info =
        (this->info & ~input::BtnMask::STATES) | input::BtnState::NORMAL;
    return;
  }

  switch (evt.mouse.left) {
  case input::MouseState::DOWN:
  case input::MouseState::HOLD:
    this->info = (this->info & ~input::BtnMask::STATES) | input::BtnState::DOWN;
    return;

  case input::MouseState::UP:
    this->info = (this->info & ~input::BtnMask::STATES) | input::BtnState::DOWN;
    data.left_click = true;
    // TODO: Just remove listener and use the InputData object instead
    if (this->left_click_listener) {
      this->left_click_listener();
    }
    return;

  case input::MouseState::NONE:
    break;
  }

  switch (evt.mouse.right) {
  case input::MouseState::DOWN:
  case input::MouseState::HOLD:
    this->info = (this->info & ~input::BtnMask::STATES) | input::BtnState::DOWN;
    break;

  case input::MouseState::UP:
    this->info = (this->info & ~input::BtnMask::STATES) | input::BtnState::DOWN;
    data.right_click = true;
    // TODO: Just remove listener and use the InputData object instead
    if (this->right_click_listener) {
      this->right_click_listener();
    }
    break;

  case input::MouseState::NONE:
    this->info =
        (this->info & ~input::BtnMask::STATES) | input::BtnState::HOVER;
    break;
  }
}

void Button::key_input(event::KeyPress& keypress) noexcept {
  if (((this->info & input::BtnMask::STATES) == input::BtnState::DISABLED) ||
      this->left_click_listener == nullptr || !this->focused) {
    keypress.to_next_section();
    return;
  }

  while (keypress.has_next()) {
    auto c = keypress.get_next_char();
    if (c == input::Keycode::ENTER) {
      this->left_click_listener();
      this->focused = false;
      break;
    }

    if (c == input::Keycode::TAB) {
      this->focused = false;
      break;
    }
  }
}

void Button::update(f32 _delta) noexcept {
  // Do nothing UwU
}

void Button::render() noexcept {
  // Draw bg
  renderer::set_color(cfg::theme::get_button_color(this->info));
  renderer::fill_rect(this->rect);

  // Draw Outline
  if (this->focused) {
    renderer::set_color({0x00, 0x00, 0x00, 0xff});
    renderer::draw_rect(this->rect);
  }

  // Draw tex
  renderer::render_texture(this->tex, this->tex_rect);
}

} // namespace view::sdl3::widget

