/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-10
 *==========================*/

#include "./preview.hpp"
#include <cmath>
#include <cstring>

namespace view::sdl3::widget {

const c8* const PLAY_IMG_PATH = "assets/play.png";
const c8* const PAUSE_IMG_PATH = "assets/pause.png";

void PreviewBox::init() noexcept {
  this->pos = {40.0F, 100.0F};
  this->size = {200.0F, 240.0F};
  this->texture_rect.pos = {44.0F, 104.0F};
  this->texture_rect.size = {192.0F, 192.0F};

  this->play_btn.pos = this->play_btn.tex_rect.pos = {44.0F, 300.0F};
  this->play_btn.size = this->play_btn.tex_rect.size = {32.0F, 32.0F};

  this->playing = false;
  this->play_btn.set_texture(renderer::load_img(PLAY_IMG_PATH));
}

error_code PreviewBox::set_anim(const draw::Anim* anim) noexcept {
  assert(anim != nullptr);

  this->anim = anim;
  this->tex = renderer::create_texture(anim->get_size());
  TRY(this->pixels.resize(anim->get_image_bytes_size()));

  return error_code::OK;
}

void PreviewBox::set_active_frame(usize active_frame) noexcept {
  this->active_frame = active_frame;

  if (this->anim && !this->playing) {
    this->set_frame_texture(active_frame);
  }
}

void PreviewBox::set_playing(bool playing) noexcept {
  if (this->anim == nullptr) {
    return;
  }

  this->playing = playing;

  if (playing) {
    this->cursor = this->anim->get_frame_count() - 1U;
    this->play_btn.set_texture(renderer::load_img(PAUSE_IMG_PATH));
  } else {
    this->set_frame_texture(this->active_frame);
    this->play_btn.set_texture(renderer::load_img(PLAY_IMG_PATH));
  }
}

void PreviewBox::update_texture() noexcept {
  assert(this->anim != nullptr);

  if (!this->playing) {
    this->set_frame_texture(this->active_frame);
  }
}

void PreviewBox::resize(const frect& rect) noexcept {
  //
}

void PreviewBox::reset() noexcept {
  //
}

void PreviewBox::locale_updated() noexcept {
  //
}

void PreviewBox::input(const event::Input& evt, InputData& data) noexcept {
  this->play_btn.input(evt, data);
  if (data.left_click) {
    data.left_click = false;
    this->set_playing(!this->playing);
    return;
  }
}

void PreviewBox::update(f32 delta) noexcept {
  if (!this->playing || this->anim == nullptr) {
    return;
  }

  this->ticks += delta;
  if (this->ticks < this->spf) {
    return;
  }

  this->cursor += this->ticks / this->spf;
  if (this->cursor >= this->anim->get_frame_count()) {
    this->cursor %= this->anim->get_frame_count();
  }
  this->ticks = std::fmod(this->ticks, this->spf);

  this->set_frame_texture(this->cursor);
}

void PreviewBox::render() noexcept {
  renderer::set_color({0x33, 0x33, 0x33, 0xff});
  renderer::fill_rect(this->rect);

  renderer::set_color({0xff, 0xff, 0xff, 0xff});
  renderer::fill_rect(this->texture_rect);

  this->play_btn.render();
  renderer::render_texture(this->tex, this->texture_rect);
}

// === Private Function === //

void PreviewBox::set_frame_texture(usize frame_index) noexcept {
  this->anim->get_flatten(
      frame_index, 0U, this->anim->get_layer_count() - 1U, this->pixels
  );
  this->tex.set_pixels(
      reinterpret_cast<rgba8*>(this->pixels.get_data()), this->anim->get_size()
  );
  std::memset(this->pixels.get_data(), 0, this->pixels.get_size());
}

} // namespace view::sdl3::widget

