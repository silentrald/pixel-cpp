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

error_code DrawBox::init_textures(ivec size) noexcept {
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
    this->textures[i] = renderer::create_texture(size);
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

error_code DrawBox::set_anim(const draw::Anim* anim) noexcept {
  assert(anim != nullptr);

  this->anim = anim;
  TRY(this->pixels.resize(anim->get_image_bytes_size()));

  return error_code::OK;
}

void DrawBox::set_active(usize frame_index, usize layer_index) noexcept {
  this->update_bot_texture(frame_index, layer_index);
  this->update_top_texture(frame_index, layer_index);
  this->update_curr_texture(
      this->anim->get_image_id(frame_index, layer_index),
      this->anim->is_layer_visible(layer_index)
  );
}

void DrawBox::update_curr_texture(usize img_id, bool show) noexcept {
  assert(this->anim != nullptr);

  if (img_id > 0U && show) {
    // NOTE: Check if get_image_fast is good here
    this->get_curr_texture().set_pixels(
        reinterpret_cast<rgba8*>(this->anim->get_image_fast(img_id).get_pixels()
        ),
        this->anim->get_size()
    );
  } else {
    this->get_curr_texture().clear(anim->get_height());
  }
}

void DrawBox::update_bot_texture(
    usize frame_index, usize layer_index
) noexcept {
  assert(this->anim != nullptr);

  std::memset(this->pixels.get_data(), 0, this->pixels.get_size());

  if (layer_index > 0) {
    this->anim->get_flatten(frame_index, 0, layer_index - 1, this->pixels);
  }

  this->get_bot_texture().set_pixels(
      reinterpret_cast<rgba8*>(this->pixels.get_data()), this->anim->get_size()
  );
}

void DrawBox::update_top_texture(
    usize frame_index, usize layer_index
) noexcept {
  assert(this->anim != nullptr);

  std::memset(this->pixels.get_data(), 0, this->pixels.get_size());

  if (layer_index < this->anim->get_layer_count() - 1) {
    this->anim->get_flatten(
        frame_index, layer_index + 1, anim->get_layer_count() - 1, this->pixels
    );
  }

  this->get_top_texture().set_pixels(
      reinterpret_cast<rgba8*>(this->pixels.get_data()), this->anim->get_size()
  );
}

void DrawBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  // TODO: UwU
}

void DrawBox::reset() noexcept {
  //
}

void DrawBox::locale_updated() noexcept {
  // Do nothing UwU
}

void DrawBox::input(const event::Input& evt, InputData& data) noexcept {
  presenter::canvas_mouse_event(evt);
}

void DrawBox::update(f32 _delta) noexcept {
  this->tick = (this->tick + 1) % 60;
}

void DrawBox::render() noexcept {
  renderer::set_color({0xff, 0xc0, 0xcb, 0xff});
  renderer::fill_rect(this->rect);

  for (i32 i = 0; i <= 4; ++i) {
    renderer::render_texture(this->textures[i], this->draw_rect);
  }
  renderer::render_texture(
      this->textures[5 + (this->tick < 30)], this->draw_rect
  );
}

} // namespace view::sdl3::widget
