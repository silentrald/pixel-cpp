/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#include "./text.hpp"

namespace view::sdl3::widget {

void Text::set_text(cfg::locale::TextId id) noexcept {
  this->id = id;
  this->update_locale();
}

void Text::reset() noexcept {
  // Do nothing UwU
}

void Text::update_locale() noexcept {
  const auto* str = cfg::locale::get_text(this->id);
  this->tex = renderer::create_text(str);
  this->rect.size = renderer::get_text_size(str);
}

void Text::input(const event::Input& evt, InputData& data) noexcept {
  // Do nothing UwU
}

void Text::update(f32 _delta) noexcept {
  // Do nothing UwU
}

void Text::render() noexcept {
  renderer::render_texture(this->tex, this->rect);
}

} // namespace view::sdl3::widget

