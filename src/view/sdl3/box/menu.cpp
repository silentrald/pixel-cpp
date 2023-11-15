/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-25
 *==========================*/

#include "./menu.hpp"

namespace view::sdl3::widget {

error_code MenuBox::push_menu_btn(MenuBtn&& btn) noexcept {
  fvec off{this->rect.pos};
  if (!this->btns.is_empty()) {
    off = this->btns.back().rect.pos;
    off.x += this->btns.back().rect.w;
  }

  btn.rect.pos = off;
  off.x += (btn.rect.w - btn.tex_rect.w) * 0.5F;
  off.y += (btn.rect.h - btn.tex_rect.h) * 0.5F;
  btn.tex_rect.pos = off;
  return this->btns.push_back(std::move(btn));
}

void MenuBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  if (this->btns.is_empty()) {
    return;
  }

  fvec off{this->rect.pos};
  fvec pad = {8.0F, 2.0F}; // NOTE: Get this from themes
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].rect.pos = off;
    this->btns[i].tex_rect.pos = {
        off.x + pad.x,
        off.y + pad.y,
    };
    off.x += this->btns[i].rect.w;
  }
}

void MenuBox::reset() noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].reset();
  }
}

void MenuBox::locale_updated(const Renderer& renderer) noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].locale_updated(renderer);
  }
}

void MenuBox::input(const event::Input& evt, Data& data) noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].input(evt, data);
  }
}

void MenuBox::update() noexcept {
  // Do nothing UwU
}

void MenuBox::render(const Renderer& renderer) const noexcept {
  // Draw bg
  renderer.set_color({0x44, 0x44, 0xff, 0xff});
  renderer.fill_rect(this->rect);

  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].render(renderer);
  }
}

} // namespace view::sdl3::widget
