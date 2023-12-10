/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-23
 *==========================*/

#include "./draw.hpp"
#include "core/ds/vector.hpp"
#include "presenter/presenter.hpp"

namespace view::sdl3::widget {

error_code
DrawBox::init_textures(const Renderer& renderer, ivec size) noexcept {
  ds::vector<rgba8> pixels{};
  TRY(pixels.resize(size.x * size.y));

  for (i32 y = 0; y < size.y; ++y) {
    for (i32 x = 0; x < size.x; ++x) {
      pixels[x + y * size.x] = (x & 0b1000) ^ (y & 0b1000)
                                   ? rgba8{0x88, 0x88, 0x88, 0xff}
                                   : rgba8{0xff, 0xff, 0xff, 0xff};
    }
  }

  for (i32 i = 0; i <= 6; ++i) {
    this->textures[i] = renderer.create_texture(size);
  }
  this->textures[0].set_pixels(pixels.get_data(), size);

  return error_code::OK;
}

Texture& DrawBox::get_bg_texture() noexcept {
  return this->textures[0];
}

Texture& DrawBox::get_bot_texture() noexcept {
  return this->textures[1];
}

Texture& DrawBox::get_curr_texture() noexcept {
  return this->textures[2];
}

Texture& DrawBox::get_empty_texture() noexcept {
  return this->textures[3];
}

Texture& DrawBox::get_top_texture() noexcept {
  return this->textures[4];
}

Texture& DrawBox::get_select1_texture() noexcept {
  return this->textures[5];
}

Texture& DrawBox::get_select2_texture() noexcept {
  return this->textures[6];
}

void DrawBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  // TODO: UwU
}

void DrawBox::reset() noexcept {
  //
}

void DrawBox::locale_updated(const Renderer& renderer) noexcept {
  // Do nothing UwU
}

void DrawBox::input(const event::Input& evt, InputData& data) noexcept {
  presenter::canvas_mouse_event(evt);
}

void DrawBox::update() noexcept {
  this->tick = (this->tick + 1) % 60;
}

void DrawBox::render(const Renderer& renderer) noexcept {
  renderer.set_color({0xff, 0xc0, 0xcb, 0xff});
  renderer.fill_rect(this->rect);

  for (i32 i = 0; i <= 4; ++i) {
    renderer.render_texture(this->textures[i], this->draw_rect);
  }
  renderer.render_texture(
      this->textures[5 + (this->tick < 30)], this->draw_rect
  );
}

} // namespace view::sdl3::widget
