/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-25
 *==========================*/

#include "./menu.hpp"
#include "core/cfg/theme.hpp"

namespace view::sdl3::widget {

const fvec PADDING = {8.0F, 2.0F};

error_code MenuBox::push_menu_btn(
    cfg::locale::TextId text_id, void (*listener)()
) noexcept {
  Text text{};
  text.set_text(text_id);

  fvec off{this->rect.pos};
  if (!this->btns.is_empty()) {
    off = this->btns.back().rect.pos;
    off.x += this->btns.back().rect.w;
  }

  MenuBtn btn{.listener = listener};
  btn.rect.pos = off;
  btn.rect.size = {.x = text.w + 16.0F, .y = text.h + 4.0F};

  text.rect.pos = off + PADDING;
  btn.text = std::move(text);

  return this->btns.push_back(std::move(btn));
}

const frect& MenuBox::get_btn_rect(usize index) const noexcept {
  return this->btns[index].rect;
}

usize MenuBox::get_btns_size() const noexcept {
  return this->btns.get_size();
}

void MenuBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  if (this->btns.is_empty()) {
    return;
  }

  fvec off{this->rect.pos};
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].rect.pos = off;
    this->btns[i].text.rect.pos = off + PADDING;
    off.x += this->btns[i].rect.w;
  }
}

void MenuBox::reset() noexcept {
  this->selected = -1;
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].state = input::BtnState::NORMAL;
  }
}

void MenuBox::update_locale() noexcept {
  fvec off = this->rect.pos;
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].text.update_locale();
    this->btns[i].rect.pos = off;
    this->btns[i].rect.size = this->btns[i].text.rect.size + PADDING * 2.0F;
  }
}

void MenuBox::input(const event::Input& evt) noexcept {
  if (!this->rect.has_point(evt.mouse.pos)) {
    for (usize i = 0; i < this->btns.get_size(); ++i) {
      this->btns[i].state = input::BtnState::NORMAL;
    }
    return;
  }

  for (usize i = 0; i < this->btns.get_size(); ++i) {
    if (!this->btns[i].rect.has_point(evt.mouse.pos)) {
      this->btns[i].state = input::BtnState::NORMAL;
      continue;
    }

    switch (evt.mouse.left) {
    case input::MouseState::NONE:
      this->btns[i].state = input::BtnState::HOVER;
      break;

    case input::MouseState::DOWN:
    case input::MouseState::HOLD:
      this->btns[i].state = input::BtnState::PRESS;
      break;

    case input::MouseState::UP:
      this->selected = i;
      if (this->btns[i].listener) {
        this->btns[i].listener();
      }
      break;
    }
  }
}

void MenuBox::update(f32 _delta) noexcept {
  // Do nothing UwU
}

void MenuBox::render() noexcept {
  // Draw bg
  renderer::set_color({0x44, 0x44, 0xff, 0xff});
  renderer::fill_rect(this->rect);

  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    renderer::set_color(cfg::theme::get_menu_btn_color(
        this->selected == i ? input::BtnState::HOVER : this->btns[i].state
    ));
    renderer::fill_rect(this->btns[i].rect);

    this->btns[i].text.render();
  }
}

} // namespace view::sdl3::widget
