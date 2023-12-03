/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-01
 *==========================*/

#include "./color_picker.hpp"

namespace view::sdl3::widget {

void ColorPicker::init(
    fvec pos, f32 size, void (*on_change)(const std::string& new_text)
) noexcept {
  this->pos = pos;
  this->size = {size, size};

  pos.x += size + 4.0F;
  this->hex_str.pos = pos;
  this->hex_str.size = {160.0F, size};
  this->hex_str.on_change = on_change;
}

void ColorPicker::set_color(rgba8 color, const Renderer& renderer) noexcept {
  this->color = color;
  this->hex_str.set_text(color::to_hex_string(color).c_str(), renderer);
}

void ColorPicker::reset() noexcept {
  //
}

void ColorPicker::locale_updated(const Renderer& renderer) noexcept {
  this->size = {renderer.get_text_height(), renderer.get_text_height()};

  this->hex_str.x = this->pos.x + renderer.get_text_height() + 4.0F;
  this->hex_str.h = renderer.get_text_height();
  this->hex_str.locale_updated(renderer);
}

void ColorPicker::input(const event::Input& evt, InputData& data) noexcept {
  this->hex_str.input(evt, data);
}

void ColorPicker::update() noexcept {
  //
}

void ColorPicker::render(const Renderer& renderer) const noexcept {
  renderer.set_color(this->color);
  renderer.fill_rect(this->rect);

  this->hex_str.render(renderer);
}

} // namespace view::sdl3::widget
