/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-27
 *==========================*/

#include "./presenter.hpp"
#include "view/sdl3/manager.hpp"

void presenter::toggle_file_ctx_menu() noexcept {
  view_.toggle_file_ctx_menu();
}

void presenter::toggle_edit_ctx_menu() noexcept {
  view_.toggle_edit_ctx_menu();
}

void presenter::open_export_ctx_menu() noexcept {
  TRY_ABORT(view_.open_export_ctx_menu(), "Could not open export context menu");
}

void presenter::close_ctx_menus() noexcept {
  view_.close_ctx_menus();
}

