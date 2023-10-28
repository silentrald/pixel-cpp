/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#include "./textbox.hpp"

namespace view::sdl3::widget {

void Textbox::update_texture(
    const Renderer& renderer, const Font& font
) noexcept {
  this->tex = std::move(renderer.create_text(font, this->text.c_str()));
  auto size = font.get_text_size(this->text.c_str());
  this->tex_rect = {
      .pos = {.x = this->rect.x + this->rect.w - size.x, .y = this->rect.y},
      .size = {.x = (f32)size.x, .y = (f32)size.y}};
}

void Textbox::reset() noexcept {
  this->focused = false;
}

void Textbox::input(const event::Input& evt) noexcept {
  if (!this->rect.has_point(evt.mouse.pos)) {
    return;
  }

  if (evt.mouse.left.state == input::MouseState::UP) {
    this->focused = true;
  }
}

void Textbox::update() noexcept {
  this->cursor_blink_tick = (this->cursor_blink_tick + 1) % 120;
}

void Textbox::render(const Renderer& renderer) const noexcept {
  renderer.set_color({0xff, 0xff, 0xff, 0xff});
  renderer.fill_rect(this->rect);

  if (this->focused) {
    renderer.render_text(this->text.c_str(), this->tex_rect.pos);
  } else {
    renderer.render_texture(this->tex, this->tex_rect);
  }

  // TODO: Render the cursor
}

} // namespace view::sdl3::widget

