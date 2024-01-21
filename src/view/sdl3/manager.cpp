/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-08
 *==========================*/

#include "./manager.hpp"
#include "./renderer.hpp"
#include "SDL_timer.h"
#include "core/cfg/locale.hpp"
#include "core/logger/logger.hpp"
#include "presenter/presenter.hpp"
#include <cstdlib>

namespace view::sdl3 {

const u64 FPS = 60;
const u64 SPF = 1000 / FPS;

error_code Manager::init() noexcept {
  this->window.init();
  renderer::init(this->window.get_window());

  this->handle_resize(this->window.size);

  TRY(this->boxes.push_back(&this->draw_box));
  TRY(this->boxes.push_back(&this->timeline_box));
  TRY(this->boxes.push_back(&this->tool_box));
  TRY(this->boxes.push_back(&this->status_box));
  TRY(this->boxes.push_back(&this->menu_box));
  TRY(this->boxes.push_back(&this->preview_box));

  // Tool box
  TRY(this->tool_box.push_btn(
      "assets/tools/pencil.png", presenter::set_pencil_tool
  ));
  TRY(this->tool_box.push_btn(
      "assets/tools/eraser.png", presenter::set_eraser_tool
  ));
  TRY(this->tool_box.push_btn("assets/tools/line.png", presenter::set_line_tool)
  );
  TRY(this->tool_box.push_btn("assets/tools/fill.png", presenter::set_fill_tool)
  );
  TRY(this->tool_box.push_btn(
      "assets/tools/fill.png", presenter::set_select_tool
  ));

  // Menu box
  fvec size{};

  TRY(this->menu_box.push_menu_btn(
      cfg::locale::TextId::FILE_, presenter::toggle_file_ctx_menu
  ));

  TRY(this->menu_box.push_menu_btn(
      cfg::locale::TextId::EDIT, presenter::toggle_edit_ctx_menu
  ));

  this->timeline_box.init();

  // NOTE: Locale
  const auto* text = "Change Language";
  this->locale_btn.set_theme(input::BtnTheme::TOOL_BTN); // TODO:
  this->locale_btn.set_texture(renderer::create_text(text));
  size = renderer::get_text_size(text);
  this->locale_btn.pos = {this->window.size.x - size.x, 0.0F};
  this->locale_btn.tex_rect.pos = {this->window.size.x - size.x, 0.0F};
  this->locale_btn.size = size;
  this->locale_btn.tex_rect.size = size;

  TRY(this->init_ctx_menus());

  this->fg_color.init(
      {40.0F, 32.0F}, renderer::get_text_height(), presenter::set_fg_color
  );
  this->bg_color.init(
      {40.0F, 40.0F + renderer::get_text_height()}, renderer::get_text_height(),
      presenter::set_bg_color
  );

  // NOTE: Preview
  this->preview_box.init();

  return error_code::OK;
}

Manager::~Manager() noexcept {
  this->clear_modals();
  renderer::destroy();
}

i32 Manager::get_window_width() const noexcept {
  return this->window.size.x;
}

i32 Manager::get_window_height() const noexcept {
  return this->window.size.y;
}

const frect& Manager::get_draw_rect() const noexcept {
  return this->draw_box.rect;
}

const frect& Manager::get_canvas_rect() const noexcept {
  return this->draw_box.draw_rect;
}

Texture& Manager::get_bg_texture() noexcept {
  return this->draw_box.get_bg_texture();
}

Texture& Manager::get_curr_texture() noexcept {
  return this->draw_box.get_curr_texture();
}

Texture& Manager::get_empty_texture() noexcept {
  return this->draw_box.get_empty_texture();
}

Texture& Manager::get_select1_texture() noexcept {
  return this->draw_box.get_select1_texture();
}

Texture& Manager::get_select2_texture() noexcept {
  return this->draw_box.get_select2_texture();
}

void Manager::update_curr_texture(usize img_id, bool show) noexcept {
  this->draw_box.update_curr_texture(img_id, show);
  this->preview_box.update_texture();
}

void Manager::update_bot_texture(
    usize frame_index, usize layer_index
) noexcept {
  this->draw_box.update_bot_texture(frame_index, layer_index);
}

void Manager::update_top_texture(
    usize frame_index, usize layer_index
) noexcept {
  this->draw_box.update_top_texture(frame_index, layer_index);
}

// === Setters === //

void Manager::set_draw_size(ivec size) noexcept {
  auto code = this->draw_box.init_textures(size);
  if (is_error(code)) {
    logger::fatal("Could not initialize draw box");
    std::abort();
  }

  this->status_box.size = size;
}

void Manager::set_canvas_rect(const frect& canvas_rect) noexcept {
  this->draw_box.draw_rect = canvas_rect;
}

void Manager::set_cursor_canvas_pos(ivec pos) noexcept {
  this->status_box.pos = pos;
}

void Manager::set_fg_color(rgba8 color) noexcept {
  this->fg_color.set_color(color);
}

void Manager::set_bg_color(rgba8 color) noexcept {
  this->bg_color.set_color(color);
}

error_code Manager::set_anim(const draw::Anim* anim) noexcept {
  TRY(this->draw_box.set_anim(anim));
  TRY(this->preview_box.set_anim(anim));
  TRY(this->timeline_box.set_anim(anim));

  return error_code::OK;
}

void Manager::set_preview_playing(bool playing) noexcept {
  this->preview_box.set_playing(playing);
}

// === Modifiers === //

error_code
Manager::insert_layer(usize index, const draw::LayerInfo& layer_info) noexcept {
  return this->timeline_box.insert_layer(index, layer_info);
}

void Manager::remove_layer(usize index) noexcept {
  return this->timeline_box.remove_layer(index);
}

void Manager::set_layer_visible(usize index, bool visible) noexcept {
  this->timeline_box.set_layer_visible(index, visible);
}

void Manager::clear_layers() noexcept {
  this->timeline_box.clear_layers();
}

void Manager::set_active_on_timeline(
    usize frame_index, usize layer_index
) noexcept {
  this->draw_box.set_active(frame_index, layer_index);
  this->timeline_box.set_active_image(frame_index, layer_index);
  this->preview_box.set_active_frame(frame_index);
}

void Manager::set_frame_offset(usize frame_offset) noexcept {
  this->timeline_box.set_frame_offset(frame_offset);
}

void Manager::run() noexcept {
  u64 start = SDL_GetTicks();
  u64 end = 0U;
  u64 duration = 0U;
  f32 delta = 0.0F;

  this->running = true;
  while (this->running) {
    this->input();
    this->update(delta);
    this->render();

    end = SDL_GetTicks();
    duration = end - start;
    if (duration < SPF) {
      // Fixed update
      delta = SPF / 1000.0F;
      SDL_Delay(SPF - duration);
      start = SDL_GetTicks();
    } else {
      // Lag update
      delta = duration / 1000.0F;
      start = end;
    }
  }
}

void Manager::reset_data() noexcept {
  data::set_selected_input(nullptr);
  data::set_new_selected_input(nullptr);
  data::set_first_input(nullptr);
  data::set_clear_selected(false);

  SDL_StopTextInput();
}

void Manager::update(f32 delta) noexcept {
  for (i32 i = 0; i < this->modals.get_size(); ++i) {
    this->modals[i]->update(delta);
  }

  for (i32 i = 0; i < this->boxes.get_size(); ++i) {
    this->boxes[i]->update(delta);
  }
}

void Manager::render() noexcept {
  renderer::set_color({0x33, 0x33, 0x33, 0xff});
  renderer::clear();

  for (usize i = 0; i < this->boxes.get_size(); ++i) {
    this->boxes[i]->render();
  }

  // TODO: Temp
  this->fg_color.render();
  this->bg_color.render();

  for (usize i = 0; i < this->modals.get_size(); ++i) {
    this->modals[i]->render();
  }

  if (this->ctx_menu_idx > -1 &&
      this->ctx_menu_idx < this->ctx_menus.get_size()) {
    this->ctx_menus[this->ctx_menu_idx].render();
  }

  for (usize i = 0; i < this->ctx_menu_stack.get_size(); ++i) {
    this->ctx_menu_stack[i].render();
  }

  this->locale_btn.render();

  renderer::present();
}

} // namespace view::sdl3
