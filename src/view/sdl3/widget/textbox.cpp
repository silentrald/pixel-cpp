/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#include "./textbox.hpp"

namespace view::sdl3::widget {

const std::string& Textbox::get_text() const noexcept {
  return this->text;
}

void Textbox::push_text(const c8* str) noexcept {
  this->text.append(str);
}

c8 Textbox::pop_char() noexcept {
  if (this->text.empty()) {
    return '\0';
  }

  auto c = this->text.back();
  this->text.pop_back();
  return c;
}

void Textbox::update_texture(
    const Renderer& renderer, const Font& font
) noexcept {
  this->tex = std::move(renderer.create_text(font, this->text.c_str()));
}

void Textbox::reposition_text_rect(const Font& font) noexcept {
  auto size = font.get_text_size(this->text.c_str());
  this->tex_rect = {
      .pos = {.x = this->rect.x + this->rect.w - size.x, .y = this->rect.y},
      .size = {.x = (f32)size.x, .y = (f32)size.y}};
}

void Textbox::reset() noexcept {
  this->focused = false;
}

void Textbox::input(const event::Input& evt, Data& data) noexcept {
  if (evt.mouse.left.state == input::MouseState::UP &&
      this->rect.has_point(evt.mouse.pos)) {
    this->focused = true;
    data.new_sel_textbox = this;
  }
}

void Textbox::update() noexcept {
  this->cursor_blink_tick = (this->cursor_blink_tick + 1) % 120;
}

void Textbox::render(const Renderer& renderer) const noexcept {
  renderer.set_color({0xff, 0xff, 0xff, 0xff});
  renderer.fill_rect(this->rect);

  if (this->focused) {
    renderer.set_color({0x00, 0x00, 0x00, 0xff});
    renderer.draw_rect(this->rect);
    renderer.render_text(this->text.c_str(), this->tex_rect.pos);
  } else {
    renderer.render_texture(this->tex, this->tex_rect);
  }

  // TODO: Render the cursor
}

} // namespace view::sdl3::widget

