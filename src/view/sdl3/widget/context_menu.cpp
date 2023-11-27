/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-24
 *==========================*/

#include "./context_menu.hpp"
#include "core/cfg/theme.hpp"
#include "core/logger/logger.hpp"
#include <algorithm>

namespace view::sdl3::widget {

error_code ContextMenu::push_item(
    cfg::locale::TextId text_id, const Renderer& renderer, void (*listener)()
) noexcept {
  widget::Text text{};
  text.set_text(text_id, renderer);

  frect rect_btn{};
  rect_btn.x = this->rect.x;
  rect_btn.y = this->rect.y + this->rect.h;
  rect_btn.h = text.rect.h + 4.0F;
  this->rect.h += rect_btn.h;

  text.rect.x = rect_btn.x + 4.0F;
  text.rect.y = rect_btn.y + 2.0F;

  if (text.rect.w + 8.0F > this->rect.w) {
    this->rect.w = rect_btn.w = text.rect.w + 8.0F;
    for (usize i = 0; i < this->items.get_size(); ++i) {
      this->items[i].rect.w = rect_btn.w;
    }
  } else {
    rect_btn.w = this->rect.w;
  }
  ++this->selected;

  return this->items.push_back(
      {.text = std::move(text),
       .rect = rect_btn,
       .state = input::BtnState::NORMAL,
       .listener = listener}
  );
}

fvec ContextMenu::get_sel_item_pos() const noexcept {
  assert(this->selected != -1);
  return {
      .x = this->items[this->selected].rect.x +
           this->items[this->selected].rect.w,
      .y = this->items[this->selected].rect.y};
}

void ContextMenu::reset() noexcept {
  this->selected = -1;
  for (usize i = 0; i < this->items.get_size(); ++i) {
    this->items[i].state = input::BtnState::NORMAL;
  }
}

void ContextMenu::locale_updated(const Renderer& renderer) noexcept {
  this->rect.size = {0.0F, 0.0F};
  for (u32 i = 0; i < this->items.get_size(); ++i) {
    this->items[i].text.locale_updated(renderer);
    // Repositioning
    this->items[i].rect.x = this->rect.x;
    this->items[i].rect.y = this->rect.y + this->rect.h;
    this->items[i].text.rect.x = this->rect.x + 4.0F;
    this->items[i].text.rect.y = this->items[i].rect.y + 2.0F;

    // Set the new heights for the text rect and context menu
    this->rect.w = std::max(this->rect.w, this->items[i].text.rect.w + 8.0F);
    this->items[i].rect.h = renderer.get_text_height() + 4.0F;
    this->rect.h += renderer.get_text_height() + 4.0F;
  }

  for (u32 i = 0; i < this->items.get_size(); ++i) {
    this->items[i].rect.w = this->rect.w;
  }
}

void ContextMenu::input(const event::Input& evt, Data& data) noexcept {
  if (!this->rect.has_point(evt.mouse.pos)) {
    for (usize i = 0; i < this->items.get_size(); ++i) {
      this->items[i].state = input::BtnState::NORMAL;
    }
    return;
  }

  for (usize i = 0; i < this->items.get_size(); ++i) {
    if (!this->items[i].rect.has_point(evt.mouse.pos)) {
      this->items[i].state = input::BtnState::NORMAL;
      continue;
    }

    switch (evt.mouse.left) {
    case input::MouseState::NONE:
      this->items[i].state = input::BtnState::HOVER;
      break;

    case input::MouseState::DOWN:
    case input::MouseState::HOLD:
      this->items[i].state = input::BtnState::DOWN;
      break;

    case input::MouseState::UP:
      this->selected = i;
      if (this->items[i].listener) {
        this->items[i].listener();
      }
      break;
    }
  }
}

void ContextMenu::update() noexcept {
  // Do nothing UwU
}

void ContextMenu::render(const Renderer& renderer) const noexcept {
  for (usize i = 0U; i < this->items.get_size(); ++i) {
    // TODO: Different theme for ctx menu btns
    renderer.set_color(cfg::theme::get_menu_btn_color(
        this->selected == i ? input::BtnState::HOVER : this->items[i].state
    ));
    renderer.fill_rect(this->items[i].rect);
    this->items[i].text.render(renderer);
  }
}

} // namespace view::sdl3::widget
