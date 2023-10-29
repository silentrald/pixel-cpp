/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#include "./file.hpp"
#include "presenter/presenter.hpp"
#include <algorithm>
#include <cstdlib>
#include <new>

namespace view::sdl3::widget {

void FileModal::init(const Renderer& renderer, const Font& font) noexcept {
  this->rect = {20.0F, 20.0F, 320.0F, 120.0F};
  ivec size{};
  fvec off = {24.0F, 24.0F};

  this->new_file_tex = renderer.create_text(font, "New File");
  size = font.get_text_size("New File");
  this->new_file_rect = {off.x, off.y, (f32)size.x, (f32)size.y};

  off.y += size.y + 4.0F;
  this->width_tex = renderer.create_text(font, "width:");
  size = font.get_text_size("width:");
  this->width_rect = {off.x, off.y, (f32)size.x, (f32)size.y};

  off.y += size.y + 4.0F;
  this->height_tex = renderer.create_text(font, "height:");
  size = font.get_text_size("height:");
  this->height_rect = {off.x, off.y, (f32)size.x, (f32)size.y};

  this->px_tex = renderer.create_text(font, "px");
  size = font.get_text_size("px");

  off.x = this->rect.x + this->rect.w - size.x - 4.0F;
  this->px_rect1 = {off.x, this->width_rect.y, (f32)size.x, (f32)size.y};
  this->px_rect2 = {off.x, this->height_rect.y, (f32)size.x, (f32)size.y};

  off.x = 32.0F + std::max(this->width_rect.w, this->height_rect.w);
  this->width_textbox.rect = {
      off.x, this->width_rect.y, 320.0F - off.x - size.x, (f32)size.y};
  this->height_textbox.rect = {
      off.x, this->height_rect.y, 320.0F - off.x - size.x, (f32)size.y};

  this->new_btn.set_theme(input::BtnTheme::TOOL_BTN); // TODO: Primary btn
  this->new_btn.set_texture(renderer.create_text(font, "New"));
  size = font.get_text_size("New");
  off.x = this->rect.x + this->rect.w - size.x - 8.0F;
  off.y = this->rect.y + this->rect.h - size.y - 8.0F;
  this->new_btn.rect = {
      off.x - 4.0F, off.y - 4.0F, (f32)size.x + 8.0F, (f32)size.y + 8.0F};
  this->new_btn.tex_rect = {off.x, off.y, (f32)size.x, (f32)size.y};
  this->new_btn.set_left_click_listener(presenter::create_anim);

  this->cancel_btn.set_theme(input::BtnTheme::TOOL_BTN); // TODO: Cancel btn
  this->cancel_btn.set_texture(renderer.create_text(font, "Cancel"));
  size = font.get_text_size("Cancel");
  off.x -= size.x + 12.0F;
  this->cancel_btn.rect = {
      off.x - 4.0F, off.y - 4.0F, (f32)size.x + 8.0F, (f32)size.y + 8.0F};
  this->cancel_btn.tex_rect = {off.x, off.y, (f32)size.x, (f32)size.y};
  this->cancel_btn.set_left_click_listener(presenter::close_modals);
}

void* FileModal::get_data() const noexcept {
  return new (std::nothrow) modal::NewFileData{
      .size = {
          atoi(this->width_textbox.get_text().c_str()),    // NOLINT
          atoi(this->height_textbox.get_text().c_str())}}; // NOLINT
}

void FileModal::resize(const frect& rect) noexcept {
  //
}

void FileModal::drag(fvec pos) noexcept {
  //
}

void FileModal::reset() noexcept {
  //
}

void FileModal::input(const event::Input& evt, Data& data) noexcept {
  this->width_textbox.input(evt, data);
  this->height_textbox.input(evt, data);

  this->new_btn.input(evt, data);
  this->cancel_btn.input(evt, data);
}

void FileModal::update() noexcept {
  bool valid = !this->width_textbox.get_text().empty() &&
               !this->height_textbox.get_text().empty();
  if (!this->new_btn.is_enabled() && valid) {
    this->new_btn.enable();
  } else if (this->new_btn.is_enabled() && !valid) {
    this->new_btn.disable();
  }
}

void FileModal::render(const Renderer& renderer) const noexcept {
  renderer.set_color({0xdd, 0xdd, 0xdd, 0xff});
  renderer.fill_rect(this->rect);

  // Texts
  renderer.render_texture(this->new_file_tex, this->new_file_rect);
  renderer.render_texture(this->width_tex, this->width_rect);
  renderer.render_texture(this->height_tex, this->height_rect);
  renderer.render_texture(this->px_tex, this->px_rect1);
  renderer.render_texture(this->px_tex, this->px_rect2);

  // Textboxes
  this->width_textbox.render(renderer);
  this->height_textbox.render(renderer);

  // Buttons
  this->new_btn.render(renderer);
  this->cancel_btn.render(renderer);
}

} // namespace view::sdl3::widget

