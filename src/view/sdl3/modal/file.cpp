/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#include "./file.hpp"
#include "core/cfg/locale.hpp"
#include "core/logger/logger.hpp"
#include "presenter/presenter.hpp"
#include <algorithm>
#include <cstdlib>
#include <new>

namespace view::sdl3::widget {

void FileModal::init(const Renderer& renderer) noexcept {
  this->rect = {20.0F, 20.0F, 320.0F, 140.0F};
  fvec off = {24.0F, 24.0F};

  this->new_file_text.rect.pos = off;
  this->new_file_text.set_text(cfg::locale::TextId::NEW_FILE, renderer);
  off.y += this->new_file_text.rect.h + 4.0F;

  this->width_text.rect.pos = off;
  this->width_text.set_text(cfg::locale::TextId::WIDTH, renderer);
  off.y += this->width_text.rect.h + 4.0F;

  this->height_text.rect.pos = off;
  this->height_text.set_text(cfg::locale::TextId::HEIGHT, renderer);
  off.y += this->height_text.rect.h + 4.0F;

  const auto* text = cfg::locale::get_text(cfg::locale::TextId::PX);
  this->px_tex = renderer.create_text(text);
  auto size = renderer.get_text_size(text);

  off.x = this->rect.x + this->rect.w - size.x - 4.0F;
  this->px_rect1 = {off.x, this->width_text.rect.y, size.x, size.y};
  this->px_rect2 = {off.x, this->height_text.rect.y, size.x, size.y};

  off.x = 32.0F + std::max(this->width_text.rect.w, this->height_text.rect.w);
  this->width_textbox.rect = {
      off.x, this->width_text.rect.y, 320.0F - off.x - size.x, (f32)size.y};
  this->height_textbox.rect = {
      off.x, this->height_text.rect.y, 320.0F - off.x - size.x, (f32)size.y};

  text = cfg::locale::get_text(cfg::locale::TextId::NEW);
  this->new_btn.set_theme(input::BtnTheme::TOOL_BTN); // TODO: Primary btn
  this->new_btn.set_texture(renderer.create_text(text));
  size = renderer.get_text_size(text);
  off.x = this->rect.x + this->rect.w - size.x - 8.0F;
  off.y = this->rect.y + this->rect.h - size.y - 8.0F;
  this->new_btn.rect = {
      off.x - 4.0F, off.y - 4.0F, (f32)size.x + 8.0F, (f32)size.y + 8.0F};
  this->new_btn.tex_rect = {off.x, off.y, (f32)size.x, (f32)size.y};
  this->new_btn.set_left_click_listener(presenter::create_anim);

  text = cfg::locale::get_text(cfg::locale::TextId::CANCEL);
  this->cancel_btn.set_theme(input::BtnTheme::TOOL_BTN); // TODO: Cancel btn
  this->cancel_btn.set_texture(renderer.create_text(text));
  size = renderer.get_text_size(text);
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

void FileModal::reset() noexcept {
  //
}

void FileModal::locale_updated(const Renderer& renderer) noexcept {
  this->new_file_text.locale_updated(renderer);
  this->width_text.locale_updated(renderer);
  this->height_text.locale_updated(renderer);

  const c8* text = nullptr;
  fvec size{};
  fvec off{};

  // Other texts
  // PX
  text = cfg::locale::get_text(cfg::locale::TextId::PX);
  this->px_tex = renderer.create_text(text);
  size = renderer.get_text_size(text);

  off.x = this->rect.x + this->rect.w - size.x - 4.0F;
  this->px_rect1 = {off.x, this->width_text.rect.y, size.x, size.y};
  this->px_rect2 = {off.x, this->height_text.rect.y, size.x, size.y};

  // Textboxes
  this->width_textbox.locale_updated(renderer);
  this->height_textbox.locale_updated(renderer);

  // Buttons
  text = cfg::locale::get_text(cfg::locale::TextId::NEW);
  this->new_btn.set_texture(renderer.create_text(text));
  size = renderer.get_text_size(text);
  off.x = this->rect.x + this->rect.w - size.x - 8.0F;
  off.y = this->rect.y + this->rect.h - size.y - 8.0F;
  this->new_btn.rect = {
      off.x - 4.0F, off.y - 4.0F, (f32)size.x + 8.0F, (f32)size.y + 8.0F};
  this->new_btn.tex_rect = {off.x, off.y, (f32)size.x, (f32)size.y};

  text = cfg::locale::get_text(cfg::locale::TextId::CANCEL);
  this->cancel_btn.set_texture(renderer.create_text(text));
  size = renderer.get_text_size(text);
  off.x -= size.x + 12.0F;
  this->cancel_btn.rect = {
      off.x - 4.0F, off.y - 4.0F, (f32)size.x + 8.0F, (f32)size.y + 8.0F};
  this->cancel_btn.tex_rect = {off.x, off.y, (f32)size.x, (f32)size.y};
}

void FileModal::drag(fvec pos) noexcept {
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
  this->new_file_text.render(renderer);
  this->width_text.render(renderer);
  this->height_text.render(renderer);
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

