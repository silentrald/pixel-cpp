/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#include "./text.hpp"

namespace view::sdl3::widget {

void Text::set_text(
    cfg::locale::TextId id, const Renderer& renderer, const Font& font
) noexcept {
  this->id = id;

  const auto* str = cfg::locale::get_text(id);
  this->tex = renderer.create_text(font, str);
  auto size = font.get_text_size(str);
  this->rect.size = {.x = (f32)size.x, .y = (f32)size.y};
}

void Text::reset() noexcept {
  // Do nothing UwU
}

void Text::input(const event::Input& evt, Data& data) noexcept {
  // Do nothing UwU
}

void Text::update() noexcept {
  // Do nothing UwU
}

void Text::render(const Renderer& renderer) const noexcept {
  renderer.render_texture(this->tex, this->rect);
}

} // namespace view::sdl3::widget

