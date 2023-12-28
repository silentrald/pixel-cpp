/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-27
 *==========================*/

#include "./manager.hpp"
#include "./widget/context_menu.hpp"
#include "core/cfg/locale.hpp"
#include "core/logger/logger.hpp"
#include "presenter/presenter.hpp"

namespace view::sdl3 {

// Cached ctx menu ids
const usize FILE_CTX_ID = 0;
const usize EDIT_CTX_ID = 1;

const usize MENU_BTN_COUNT = 2;

// NOTE: Adjust these when ctx menus for menu box are added
const usize LAYERS_CTX_ID = 2;
const usize TIMELINE_CTX_ID = 3;

const usize CTX_MENUS_COUNT = 4;

error_code Manager::init_ctx_menus() noexcept {
  TRY(this->ctx_menus.resize(CTX_MENUS_COUNT));

  // File Context Menu
  {
    auto& file_ctx_menu = this->ctx_menus[FILE_CTX_ID];
    file_ctx_menu.pos = {
        this->menu_box.get_btn_rect(FILE_CTX_ID).x, this->menu_box.h};

    TRY(file_ctx_menu.push_item(cfg::locale::TextId::NEW, []() {
      presenter::close_ctx_menus();
      presenter::new_file();
    }));
    TRY(file_ctx_menu.push_item(cfg::locale::TextId::OPEN, []() {
      presenter::close_ctx_menus();
      presenter::open_file();
    }));
    TRY(file_ctx_menu.push_item(cfg::locale::TextId::SAVE, []() {
      presenter::close_ctx_menus();
      presenter::save_file();
    }));
    TRY(file_ctx_menu.push_item(cfg::locale::TextId::SV_AS, []() {
      presenter::close_ctx_menus();
      logger::debug("Not implemented yet UwU"); // TODO:
    }));
    TRY(file_ctx_menu.push_item(
        cfg::locale::TextId::EXPORT, presenter::open_export_ctx_menu
    ));
  }

  // Edit Context Menu
  {
    auto& edit_ctx_menu = this->ctx_menus[EDIT_CTX_ID];
    edit_ctx_menu.pos = {
        this->menu_box.get_btn_rect(EDIT_CTX_ID).x, this->menu_box.h};
    TRY(edit_ctx_menu.push_item(cfg::locale::TextId::UNDO, []() {
      presenter::close_ctx_menus();
      presenter::undo_action();
    }));
    TRY(edit_ctx_menu.push_item(cfg::locale::TextId::REDO, []() {
      presenter::close_ctx_menus();
      presenter::redo_action();
    }));
  }

  // Layers Context Menu
  {
    auto& layers_ctx_menu = this->ctx_menus[LAYERS_CTX_ID];
    TRY(layers_ctx_menu.push_item(cfg::locale::TextId::INS_LAYER, []() {
      presenter::close_ctx_menus();
      presenter::insert_at_selected_layer();
    }));
    TRY(layers_ctx_menu.push_item(cfg::locale::TextId::REM_LAYER, []() {
      presenter::close_ctx_menus();
      presenter::remove_at_selected_layer();
    }));
    TRY(layers_ctx_menu.push_item(cfg::locale::TextId::PROPERTIES, []() {
      presenter::close_ctx_menus();
      logger::debug("UwU"); // TODO:
    }));
  }

  // Timeline Content Menu
  {
    auto& timeline_ctx_menu = this->ctx_menus[TIMELINE_CTX_ID];
    TRY(timeline_ctx_menu.push_item(cfg::locale::TextId::BLANK_FRAME_INS, []() {
      presenter::close_ctx_menus();
      presenter::insert_at_selected_frame();
    }));
    TRY(timeline_ctx_menu.push_item(cfg::locale::TextId::REM_FRAME, []() {
      presenter::close_ctx_menus();
      presenter::remove_at_selected_frame();
    }));
  }

  return error_code::OK;
}

void Manager::handle_ctx_menu_event() noexcept {
  // Check if the mouse up event is outside any ctx menu rect
  // If it is outside, close all ctx menus
  if (this->input_evt.mouse.left == input::MouseState::UP) {
    bool outside = true;

    if (this->ctx_menus[this->ctx_menu_idx].rect.has_point(
            this->input_evt.mouse.pos
        )) {
      outside = false;
    } else {
      for (usize i = 0; i < this->ctx_menu_stack.get_size(); ++i) {
        if (this->ctx_menu_stack[i].rect.has_point(this->input_evt.mouse.pos)) {
          outside = false;
          break;
        }
      }
    }

    if (outside) {
      this->close_ctx_menus();
      return;
    }
  }

  if (this->ctx_menu_idx < MENU_BTN_COUNT) {
    for (usize i = 0; i < MENU_BTN_COUNT; ++i) {
      if (this->menu_box.get_btn_rect(i).has_point(this->input_evt.mouse.pos)) {
        if (this->ctx_menu_idx == i) {
          break;
        }

        this->ctx_menus[this->ctx_menu_idx].reset();
        this->ctx_menu_idx = i;
        this->menu_box.selected = i;
        this->ctx_menu_stack.clear();
        break;
      }
    }
  }

  this->ctx_menus[this->ctx_menu_idx].input(this->input_evt, this->data);
  for (usize i = 0; i < this->ctx_menu_stack.get_size(); ++i) {
    this->ctx_menu_stack[i].input(this->input_evt, this->data);
  }
}

void Manager::handle_ctx_menu_update_locale() noexcept {
  for (usize i = 0; i < this->ctx_menus.get_size(); ++i) {
    if (i < MENU_BTN_COUNT) {
      this->ctx_menus[i].x = this->menu_box.get_btn_rect(i).x,
      this->ctx_menus[i].y = this->menu_box.y + this->menu_box.h;
    }
    this->ctx_menus[i].update_locale();
  }

  for (usize i = 0; i < this->ctx_menu_stack.get_size(); ++i) {
    this->ctx_menu_stack[i].pos =
        i == 0 ? this->ctx_menus[this->ctx_menu_idx].get_sel_item_pos()
               : this->ctx_menu_stack[i - 1].get_sel_item_pos();
    this->ctx_menu_stack[i].update_locale();
  }
}

void Manager::toggle_file_ctx_menu() noexcept {
  this->ctx_menu_idx = this->ctx_menu_idx == 0 ? -1 : 0;
}

void Manager::toggle_edit_ctx_menu() noexcept {
  this->ctx_menu_idx = this->ctx_menu_idx == 1 ? -1 : 1;
}

error_code Manager::open_export_ctx_menu() noexcept {
  assert(this->ctx_menu_idx == FILE_CTX_ID);

  widget::ContextMenu ctx_menu{};
  ctx_menu.pos = this->ctx_menus[FILE_CTX_ID].get_sel_item_pos();
  TRY(ctx_menu.push_item(cfg::locale::TextId::PNG_EXPORT, []() {
    presenter::close_ctx_menus();
    presenter::export_to_png();
  }));
  TRY(ctx_menu.push_item(cfg::locale::TextId::JPG_EXPORT, nullptr));

  return this->ctx_menu_stack.push_back(std::move(ctx_menu));
}

void Manager::open_layers_ctx_menu() noexcept {
  this->ctx_menu_stack.clear();

  this->ctx_menus[LAYERS_CTX_ID].reposition(this->input_evt.mouse.pos);
  // Remove Button
  /* this->ctx_menus[LAYERS_CTX_ID].set_disabled(1U, this->) */
  this->ctx_menu_idx = LAYERS_CTX_ID;
}

void Manager::open_timeline_ctx_menu() noexcept {
  this->ctx_menu_stack.clear();

  this->ctx_menus[TIMELINE_CTX_ID].reposition(this->input_evt.mouse.pos);
  this->ctx_menu_idx = TIMELINE_CTX_ID;
}

void Manager::close_ctx_menus() noexcept {
  if (this->ctx_menu_idx > -1) {
    this->ctx_menus[this->ctx_menu_idx].reset();
    this->ctx_menu_idx = -1;
  }
  this->ctx_menu_stack.clear();
}

} // namespace view::sdl3
