/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-27
 *==========================*/

#include "./manager.hpp"
#include "./widget/context_menu.hpp"
#include "core/cfg/locale.hpp"
#include "presenter/presenter.hpp"

namespace view::sdl3 {

// Cached ctx menu ids
const usize FILE_CTX_ID = 0;
const usize EDIT_CTX_ID = 1;

error_code Manager::init_ctx_menus() noexcept {
  TRY(this->ctx_menus.resize(2));

  // File Context Menu
  {
    auto& file_ctx_menu = this->ctx_menus[FILE_CTX_ID];
    file_ctx_menu.pos = {
        this->menu_box.get_btn_rect(FILE_CTX_ID).x, this->menu_box.h};

    TRY(file_ctx_menu.push_item(cfg::locale::TextId::NEW, this->renderer, []() {
      presenter::close_ctx_menus();
      presenter::new_file_clicked();
    }));
    TRY(file_ctx_menu
            .push_item(cfg::locale::TextId::OPEN, this->renderer, []() {
              presenter::close_ctx_menus();
              presenter::open_file();
            }));
    TRY(file_ctx_menu
            .push_item(cfg::locale::TextId::SAVE, this->renderer, []() {
              presenter::close_ctx_menus();
              presenter::save_file();
            }));
    TRY(file_ctx_menu.push_item(
        cfg::locale::TextId::SV_AS, this->renderer, nullptr
    ));
    TRY(file_ctx_menu.push_item(
        cfg::locale::TextId::EXPORT, this->renderer,
        presenter::open_export_ctx_menu
    ));
  }

  // Edit Context Menu
  {
    auto& edit_ctx_menu = this->ctx_menus[EDIT_CTX_ID];
    edit_ctx_menu.pos = {
        this->menu_box.get_btn_rect(EDIT_CTX_ID).x, this->menu_box.h};
    TRY(edit_ctx_menu
            .push_item(cfg::locale::TextId::UNDO, this->renderer, []() {
              presenter::close_ctx_menus();
              presenter::undo_action();
            }));
    TRY(edit_ctx_menu
            .push_item(cfg::locale::TextId::REDO, this->renderer, []() {
              presenter::close_ctx_menus();
              presenter::redo_action();
            }));
  }

  return error_code::OK;
}

void Manager::handle_ctx_menu_event() noexcept {
  // Check if the mouse up event is outside any ctx menu rect
  // If it is outside, close all ctx menus
  if (this->input_evt.mouse.left.state == input::MouseState::UP) {
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

  for (usize i = 0; i < this->menu_box.get_btns_size(); ++i) {
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

  this->ctx_menus[this->ctx_menu_idx].input(this->input_evt, this->data);
  for (usize i = 0; i < this->ctx_menu_stack.get_size(); ++i) {
    this->ctx_menu_stack[i].input(this->input_evt, this->data);
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
  TRY(ctx_menu.push_item(cfg::locale::TextId::PNG_EXPORT, this->renderer, []() {
    presenter::close_ctx_menus();
    presenter::export_to_png();
  }));
  TRY(ctx_menu.push_item(
      cfg::locale::TextId::JPG_EXPORT, this->renderer, nullptr
  ));

  return this->ctx_menu_stack.push_back(std::move(ctx_menu));
}

void Manager::close_ctx_menus() noexcept {
  if (this->ctx_menu_idx > -1) {
    this->ctx_menus[this->ctx_menu_idx].reset();
    this->ctx_menu_idx = -1;
  }
  this->ctx_menu_stack.clear();
}

} // namespace view::sdl3
