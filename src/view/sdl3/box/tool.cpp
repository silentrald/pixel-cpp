/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-23
 *==========================*/

#include "./tool.hpp"
#include "presenter/presenter.hpp"

namespace view::sdl3::widget {

inline const f32 TOOL_BTN_SIZE = 32.0F;

error_code ToolBox::push_btn(Button&& btn) noexcept {
  fvec off{this->rect.pos};
  if (!this->btns.is_empty()) {
    off = this->btns.back().rect.pos;
    if (off.x + TOOL_BTN_SIZE >= this->rect.x) {
      off.y += TOOL_BTN_SIZE;
    } else {
      off.x += TOOL_BTN_SIZE;
    }
  }
  btn.rect = {.pos = off, .size = {TOOL_BTN_SIZE, TOOL_BTN_SIZE}};
  btn.tex_rect = {.pos = off, .size = {TOOL_BTN_SIZE, TOOL_BTN_SIZE}};
  btn.set_theme(input::BtnTheme::TOOL_BTN);
  return this->btns.push_back(std::move(btn));
}

void ToolBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  if (this->btns.is_empty()) {
    return;
  }

  fvec off{this->rect.pos};
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].rect.pos = this->btns[i].tex_rect.pos = off;

    if (off.x + TOOL_BTN_SIZE >= this->rect.x) {
      off.x = this->rect.x;
      off.y += TOOL_BTN_SIZE;
    } else {
      off.x += TOOL_BTN_SIZE;
    }
  }
}

void ToolBox::reset() noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].reset();
  }
}

void ToolBox::update_locale() noexcept {
  // NOTE: Do nothing for now UwU, just add logic here once tool tips/hints are
  // created
}

void ToolBox::input(const event::Input& evt, InputData& data) noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].input(evt, data);
  }
}

void ToolBox::update(f32 delta) noexcept {
  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].update(delta);
  }
}

void ToolBox::render() noexcept {
  renderer::set_color({0x66, 0x33, 0x33, 0xff});
  renderer::fill_rect(this->rect);

  for (i32 i = 0; i < this->btns.get_size(); ++i) {
    this->btns[i].render();
  }
}

} // namespace view::sdl3::widget
