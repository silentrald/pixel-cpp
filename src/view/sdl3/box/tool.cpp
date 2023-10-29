/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-23
 *==========================*/

#include "./tool.hpp"
#include "presenter/presenter.hpp"

namespace view::sdl3::widget {

void ToolBox::push_btn(Button&& btn) noexcept {
  fvec off{this->rect.pos};
  if (!this->btns.is_empty()) {
    off = this->btns.back().rect.pos;
    if (off.x + 32.0F >= this->rect.x) {
      off.y += 32.0F;
    } else {
      off.x += 32.0F;
    }
  }
  btn.rect = {.pos = off, .size = {32.0F, 32.0F}};
  btn.tex_rect = {.pos = off, .size = {32.0F, 32.0F}};
  btn.set_theme(input::BtnTheme::TOOL_BTN);
  this->btns.push_back(std::move(btn));
}

void ToolBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  // TODO: UwU
}

void ToolBox::reset() noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].reset();
  }
}

void ToolBox::input(const event::Input& evt, Data& data) noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].input(evt, data);
  }
}

void ToolBox::update() noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].update();
  }
}

void ToolBox::render(const Renderer& renderer) const noexcept {
  renderer.set_color({0x66, 0x33, 0x33, 0xff});
  renderer.fill_rect(this->rect);

  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].render(renderer);
  }
}

} // namespace view::sdl3::widget
