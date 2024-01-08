/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-23
 *==========================*/

#include "./tool.hpp"

namespace view::sdl3::widget {

inline const f32 TOOL_BTN_SIZE = 32.0F;

error_code
ToolBox::push_btn(const c8* texture_path, void (*on_left_click)()) noexcept {
  Item item{};
  item.btn.set_theme(input::BtnTheme::TOOL_BTN);
  item.btn.set_texture(renderer::load_img(texture_path));

  fvec off{this->rect.pos};
  if (!this->items.is_empty()) {
    off = this->items.back().btn.rect.pos;
    if (off.x + TOOL_BTN_SIZE >= this->rect.x) {
      off.y += TOOL_BTN_SIZE;
    } else {
      off.x += TOOL_BTN_SIZE;
    }
  }
  item.btn.rect = {.pos = off, .size = {TOOL_BTN_SIZE, TOOL_BTN_SIZE}};
  item.btn.tex_rect = {.pos = off, .size = {TOOL_BTN_SIZE, TOOL_BTN_SIZE}};
  item.btn.set_theme(input::BtnTheme::TOOL_BTN);
  return this->items.push_back(std::move(item));
}

void ToolBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  if (this->items.is_empty()) {
    return;
  }

  fvec off{this->rect.pos};
  for (i32 i = 0; i < this->items.get_size(); ++i) {
    this->items[i].btn.rect.pos = this->items[i].btn.tex_rect.pos = off;

    if (off.x + TOOL_BTN_SIZE >= this->rect.x) {
      off.x = this->rect.x;
      off.y += TOOL_BTN_SIZE;
    } else {
      off.x += TOOL_BTN_SIZE;
    }
  }
}

void ToolBox::reset() noexcept {
  for (i32 i = 0; i < this->items.get_size(); ++i) {
    this->items[i].btn.reset();
  }
}

void ToolBox::update_locale() noexcept {
  // NOTE: Do nothing for now UwU, just add logic here once tool tips/hints are
  // created
}

void ToolBox::input(const event::Input& evt) noexcept {
  for (i32 i = 0; i < this->items.get_size(); ++i) {
    this->items[i].btn.input(evt);
    if (data::is_left_click()) {
      this->items[i].on_left_click();
    }
  }
}

void ToolBox::update(f32 delta) noexcept {
  for (i32 i = 0; i < this->items.get_size(); ++i) {
    this->items[i].btn.update(delta);
  }
}

void ToolBox::render() noexcept {
  renderer::set_color({0x66, 0x33, 0x33, 0xff});
  renderer::fill_rect(this->rect);

  for (i32 i = 0; i < this->items.get_size(); ++i) {
    this->items[i].btn.render();
  }
}

} // namespace view::sdl3::widget
