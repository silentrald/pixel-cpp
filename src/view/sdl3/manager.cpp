/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-08
 *==========================*/

#include "./manager.hpp"
#include "./widget/menu_btn.hpp"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL_blendmode.h"
#include "SDL_init.h"
#include "SDL_mouse.h"
#include "SDL_oldnames.h"
#include "SDL_rect.h"
#include "SDL_timer.h"
#include "core/cfg/locale.hpp"
#include "core/logger/logger.hpp"
#include "presenter/presenter.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <new>

namespace view::sdl3 {

const u64 FPS = 60;
const u64 SPF = 1000 / FPS;

void Manager::init() noexcept {
  this->window.init();

  this->font.init(cfg::locale::get_font(), cfg::locale::get_size());
  this->renderer.init(this->window.get_window(), this->font);

  this->handle_resize(this->window.size);

  this->boxes.push_back(&this->draw_box);
  this->boxes.push_back(&this->tool_box);
  this->boxes.push_back(&this->status_box);
  this->boxes.push_back(&this->menu_box);

  // Tool box
  widget::Button btn{};

  btn.set_theme(input::BtnTheme::TOOL_BTN);
  btn.set_texture(this->renderer.load_img("assets/tools/pencil.png"));
  btn.set_left_click_listener(presenter::set_pencil_tool);
  this->tool_box.push_btn(std::move(btn));

  btn.set_theme(input::BtnTheme::TOOL_BTN);
  btn.set_texture(this->renderer.load_img("assets/tools/eraser.png"));
  btn.set_left_click_listener(presenter::set_eraser_tool);
  this->tool_box.push_btn(std::move(btn));

  btn.set_theme(input::BtnTheme::TOOL_BTN);
  btn.set_texture(this->renderer.load_img("assets/tools/line.png"));
  btn.set_left_click_listener(presenter::set_line_tool);
  this->tool_box.push_btn(std::move(btn));

  btn.set_theme(input::BtnTheme::TOOL_BTN);
  btn.set_texture(this->renderer.load_img("assets/tools/fill.png"));
  btn.set_left_click_listener(presenter::set_fill_tool);
  this->tool_box.push_btn(std::move(btn));

  btn.set_theme(input::BtnTheme::TOOL_BTN);
  btn.set_texture(this->renderer.load_img("assets/tools/fill.png"));
  btn.set_left_click_listener(presenter::set_select_tool);
  this->tool_box.push_btn(std::move(btn));

  // Menu box
  ivec size{};
  widget::MenuBtn menu_btn{};

  const c8* text = cfg::locale::get_text(cfg::locale::TextId::FILE_MENU_ITEM);
  menu_btn.set_texture(this->renderer.create_text(this->font, text));
  size = this->font.get_text_size(text);
  menu_btn.tex_rect.size = {(f32)size.x, (f32)size.y};
  menu_btn.rect.size = {(f32)size.x + 16.0F, (f32)size.y + 4.0F};
  menu_btn.set_left_click_listener(presenter::new_file_clicked);
  this->menu_box.push_menu_btn(std::move(menu_btn));

  text = cfg::locale::get_text(cfg::locale::TextId::EDIT_MENU_ITEM);
  menu_btn.set_texture(this->renderer.create_text(this->font, text));
  size = this->font.get_text_size(text);
  menu_btn.tex_rect.size = {(f32)size.x, (f32)size.y};
  menu_btn.rect.size = {(f32)size.x + 16.0F, (f32)size.y + 4.0F};
  menu_btn.set_left_click_listener(presenter::debug_callback); // TODO:
  this->menu_box.push_menu_btn(std::move(menu_btn));

  text = cfg::locale::get_text(cfg::locale::TextId::VIEW_MENU_ITEM);
  menu_btn.set_texture(this->renderer.create_text(this->font, text));
  size = this->font.get_text_size(text);
  menu_btn.tex_rect.size = {(f32)size.x, (f32)size.y};
  menu_btn.rect.size = {(f32)size.x + 16.0F, (f32)size.y + 4.0F};
  menu_btn.set_left_click_listener(presenter::debug_callback); // TODO:
  this->menu_box.push_menu_btn(std::move(menu_btn));
}

Manager::~Manager() noexcept {
  this->clear_modals();
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

void Manager::set_draw_size(ivec size) noexcept {
  this->draw_box.init_textures(this->renderer, size);
  this->status_box.size = size;
}

void Manager::set_canvas_rect(const frect& canvas_rect) noexcept {
  this->draw_box.draw_rect = canvas_rect;
}

void Manager::set_cursor_canvas_pos(ivec pos) noexcept {
  this->status_box.pos = pos;
}

Texture& Manager::get_bg_texture() noexcept {
  return this->draw_box.get_bg_texture();
}

Texture& Manager::get_bot_texture() noexcept {
  return this->draw_box.get_bot_texture();
}

Texture& Manager::get_curr_texture() noexcept {
  return this->draw_box.get_curr_texture();
}

Texture& Manager::get_empty_texture() noexcept {
  return this->draw_box.get_empty_texture();
}

Texture& Manager::get_top_texture() noexcept {
  return this->draw_box.get_top_texture();
}

Texture& Manager::get_select1_texture() noexcept {
  return this->draw_box.get_select1_texture();
}

Texture& Manager::get_select2_texture() noexcept {
  return this->draw_box.get_select2_texture();
}

void Manager::run() noexcept {
  u64 start = SDL_GetTicks();
  u64 duration = 0U;

  this->running = true;
  while (this->running) {
    this->input();
    this->update();
    this->render();

    duration = SDL_GetTicks() - start;
    if (duration < SPF) {
      SDL_Delay(SPF - duration);
    }
    start = SDL_GetTicks();
  }
}

void Manager::reset_data() noexcept {
  this->data.sel_textbox = nullptr;
  this->data.new_sel_textbox = nullptr;

  SDL_StopTextInput();
}

void Manager::update() noexcept {
  for (i32 i = 0; i < this->modals.get_size(); ++i) {
    this->modals[i]->update();
  }

  for (i32 i = 0; i < this->boxes.get_size(); ++i) {
    this->boxes[i]->update();
  }
}

void Manager::render() noexcept {
  this->renderer.set_color({0x33, 0x33, 0x33, 0xff});
  this->renderer.clear();

  for (i32 i = 0; i < this->boxes.get_size(); ++i) {
    this->boxes[i]->render(this->renderer);
  }

  for (i32 i = 0; i < this->modals.get_size(); ++i) {
    this->modals[i]->render(this->renderer);
  }

  this->renderer.present();
}

} // namespace view::sdl3

