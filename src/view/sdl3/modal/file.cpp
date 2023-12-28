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

const f32 FILE_MODAL_WIDTH = 320.0F;
const f32 FILE_MODAL_HEIGHT = 144.0F;

void FileModal::init(fvec window_size, InputData& data) noexcept {
  this->x = (window_size.x - FILE_MODAL_WIDTH) * 0.5F;
  this->y = (window_size.y - FILE_MODAL_HEIGHT) * 0.5F;
  this->size = {FILE_MODAL_WIDTH, FILE_MODAL_HEIGHT};
  this->title_rect.pos = this->pos;
  this->title_rect.size = {
      FILE_MODAL_WIDTH, renderer::get_text_height() + 8.0F};
  fvec off = this->pos + fvec{4.0F, 4.0F};

  this->title_text.pos = off;
  this->title_text.set_text(cfg::locale::TextId::NEW_FILE);
  off.y += this->title_text.h + 12.0F;

  this->width_text.pos = off;
  this->width_text.set_text(cfg::locale::TextId::WIDTH);
  off.y += this->width_text.h + 8.0F;

  this->height_text.pos = off;
  this->height_text.set_text(cfg::locale::TextId::HEIGHT);
  off.y += this->height_text.h + 4.0F;

  const auto* text = cfg::locale::get_text(cfg::locale::TextId::PX);
  this->px_tex = renderer::create_text(text);
  auto size = renderer::get_text_size(text);

  off.x = this->x + this->w - size.x - 4.0F;
  this->px_rect1 = {off.x, this->width_text.y, size.x, size.y};
  this->px_rect2 = {off.x, this->height_text.y, size.x, size.y};

  off.x = this->x + 8.0F + std::max(this->width_text.w, this->height_text.w);
  this->width_textbox.pos = {off.x, this->width_text.y};
  this->width_textbox.size = {this->px_rect1.x - off.x - 8.0F, size.y};
  this->width_textbox.next_input = &this->height_textbox;

  this->height_textbox.pos = {off.x, this->height_text.y};
  this->height_textbox.size = {this->px_rect1.x - off.x - 8.0F, size.y};
  this->height_textbox.next_input = &this->cancel_btn;

  text = cfg::locale::get_text(cfg::locale::TextId::NEW);
  this->new_btn.set_theme(input::BtnTheme::TOOL_BTN); // TODO: Primary btn
  this->new_btn.set_texture(renderer::create_text(text));
  size = renderer::get_text_size(text);
  off.x = this->x + this->w - size.x - 8.0F;
  off.y = this->y + this->h - size.y - 8.0F;
  this->new_btn.pos = off - fvec{4.0F, 4.0F};
  this->new_btn.size = size + fvec{8.0F, 8.0F};
  this->new_btn.tex_rect = {.pos = off, .size = size};
  this->new_btn.set_left_click_listener(presenter::create_anim);

  text = cfg::locale::get_text(cfg::locale::TextId::CANCEL);
  this->cancel_btn.set_theme(input::BtnTheme::TOOL_BTN); // TODO: Cancel btn
  this->cancel_btn.set_texture(renderer::create_text(text));
  size = renderer::get_text_size(text);
  off.x -= size.x + 12.0F;
  this->cancel_btn.pos = off - fvec{4.0F, 4.0F};
  this->cancel_btn.size = size + fvec{8.0F, 8.0F};
  this->cancel_btn.tex_rect = {.pos = off, .size = size};
  this->cancel_btn.set_left_click_listener(presenter::close_modals);
  this->cancel_btn.next_input = &this->new_btn;

  data.first_input = &this->width_textbox;
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

void FileModal::reposition(fvec pos) noexcept {
  auto off = pos - this->pos;
  this->pos = pos;
  this->title_rect.pos = pos;

  this->title_text.pos += off;
  this->width_text.pos += off;
  this->height_text.pos += off;
  this->px_rect1.pos += off;
  this->px_rect2.pos += off;

  this->width_textbox.pos += off;
  this->width_textbox.tex_rect.pos += off;
  this->height_textbox.pos += off;
  this->height_textbox.tex_rect.pos += off;

  this->new_btn.pos += off;
  this->new_btn.tex_rect.pos += off;
  this->cancel_btn.pos += off;
  this->cancel_btn.tex_rect.pos += off;
}

void FileModal::reset() noexcept {
  //
}

void FileModal::update_locale() noexcept {
  this->title_text.update_locale();
  this->width_text.update_locale();
  this->height_text.update_locale();

  const c8* text = nullptr;
  fvec size{};
  fvec off{};

  // Other texts
  // PX
  text = cfg::locale::get_text(cfg::locale::TextId::PX);
  this->px_tex = renderer::create_text(text);
  size = renderer::get_text_size(text);

  off.x = this->x + this->w - size.x - 4.0F;
  this->px_rect1 = {off.x, this->width_text.y, size.x, size.y};
  this->px_rect2 = {off.x, this->height_text.y, size.x, size.y};

  // Textboxes
  this->width_textbox.update_locale();
  this->height_textbox.update_locale();

  // Buttons
  text = cfg::locale::get_text(cfg::locale::TextId::NEW);
  this->new_btn.set_texture(renderer::create_text(text));
  size = renderer::get_text_size(text);
  off.x = this->x + this->w - size.x - 8.0F;
  off.y = this->y + this->h - size.y - 8.0F;
  this->new_btn.rect = {
      off.x - 4.0F, off.y - 4.0F, size.x + 8.0F, size.y + 8.0F};
  this->new_btn.tex_rect = {.pos = off, .size = size};

  text = cfg::locale::get_text(cfg::locale::TextId::CANCEL);
  this->cancel_btn.set_texture(renderer::create_text(text));
  size = renderer::get_text_size(text);
  off.x -= size.x + 12.0F;
  this->cancel_btn.rect = {
      off.x - 4.0F, off.y - 4.0F, size.x + 8.0F, size.y + 8.0F};
  this->cancel_btn.tex_rect = {.pos = off, .size = size};
}

void FileModal::input(const event::Input& evt, InputData& data) noexcept {
  if (data.dragging) {
    this->reposition(data.orig_pos + evt.mouse.pos - data.orig_mouse);
    if (evt.mouse.left != input::MouseState::HOLD) {
      data.dragging = false;
    }
    return;
  }

  if (evt.mouse.left == input::MouseState::DOWN &&
      this->title_rect.has_point(evt.mouse.pos)) {
    data.orig_mouse = evt.mouse.pos;
    data.orig_pos = this->pos;
    data.dragging = true;
    return;
  }

  this->width_textbox.input(evt, data);
  this->height_textbox.input(evt, data);

  this->new_btn.input(evt, data);
  this->cancel_btn.input(evt, data);
}

void FileModal::update(f32 _delta) noexcept {
  bool valid = !this->width_textbox.get_text().empty() &&
               !this->height_textbox.get_text().empty();
  if (!this->new_btn.is_enabled() && valid) {
    this->new_btn.enable();
  } else if (this->new_btn.is_enabled() && !valid) {
    this->new_btn.disable();
  }
}

void FileModal::render() noexcept {
  renderer::set_color({0xdd, 0xdd, 0xdd, 0xff});
  renderer::fill_rect(this->rect);

  renderer::set_color({0xbb, 0xbb, 0xbb, 0xff});
  renderer::fill_rect(this->title_rect);

  // Texts
  this->title_text.render();
  this->width_text.render();
  this->height_text.render();
  renderer::render_texture(this->px_tex, this->px_rect1);
  renderer::render_texture(this->px_tex, this->px_rect2);

  // Textboxes
  this->width_textbox.render();
  this->height_textbox.render();

  // Buttons
  this->new_btn.render();
  this->cancel_btn.render();
}

} // namespace view::sdl3::widget

