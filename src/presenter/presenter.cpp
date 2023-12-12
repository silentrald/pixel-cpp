/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-09
 *==========================*/

#include "./presenter.hpp"
#include "core/cfg/locale.hpp"
#include "core/cfg/shortcut.hpp"
#include "core/draw/types.hpp"
#include "core/ds/vector.hpp"
#include "core/file/pxl.hpp"
#include "core/history/action.hpp"
#include "core/history/caretaker.hpp"
#include "core/logger/logger.hpp"
#include "model/model.hpp"
#include "types.hpp"
#include "view/modal.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void presenter::init() noexcept {
  TRY_ABORT(
      cfg::locale::load_locale(model_.locale), "Could not load initial locale"
  );

  TRY_ABORT(
      shortcut_.load_config("keys.ini"),
      "Could not load initial shortcut key mapping"
  );

  TRY_ABORT(caretaker_.init(50), "Could not init caretaker");

  pxl_.set_auto_save(50U);
  TRY_ABORT(view_.init(), "Could not init view");
  view_.set_fg_color(model_.fg_color);
  view_.set_bg_color(model_.bg_color);
}

void presenter::run() noexcept {
  view_.run();

  view_.~View();
  model_.~Model();
}

// === Events === //

void presenter::set_locale() noexcept {
  logger::info("Language Change");

  using namespace cfg::locale;
  model_.locale =
      model_.locale == Locale::ENGLISH ? Locale::JAPANESE : Locale::ENGLISH;

  if (is_error(load_locale(model_.locale))) {
    logger::fatal("Could not load locale");
    std::abort();
  }
  view_.locale_updated();
}

void presenter::window_resized() noexcept {
  model_.bounds = view_.get_draw_rect();

  // Clamping
  model_.rect.x = std::clamp(
      model_.rect.x, model_.bounds.x - model_.rect.w + 32.0F,
      model_.bounds.x + model_.bounds.w - 32.0F
  );
  model_.rect.y = std::clamp(
      model_.rect.y, model_.bounds.y - model_.rect.h + 32.0F,
      model_.bounds.y + model_.bounds.h - 32.0F
  );
  view_.set_canvas_rect(model_.rect);
}

inline void handle_unselect() noexcept {
  logger::info("Unselect");
  std::fill(model_.select_mask.begin(), model_.select_mask.end(), true);

  view_.get_select1_texture().clear(model_.anim.get_height());
  view_.get_select2_texture().clear(model_.anim.get_height());
}

void presenter::key_down_event(
    input::Keycode keycode, input::KeyMod key_mod
) noexcept {
  if (model_.is_editing_image) {
    return;
  }

  using namespace input;
  switch (shortcut_.get_shortcut_key(
      keycode | (key_mod.ctrl * input::Keycode::CTRL) |
      (key_mod.shift * input::Keycode::SHIFT) |
      (key_mod.alt * input::Keycode::ALT)
  )) {
  case cfg::ShortcutKey::TOOL_PENCIL:
    presenter::set_pencil_tool();
    break;

  case cfg::ShortcutKey::TOOL_ERASER:
    presenter::set_eraser_tool();
    break;

  case cfg::ShortcutKey::TOOL_LINE:
    presenter::set_line_tool();
    break;

  case cfg::ShortcutKey::TOOL_FILL:
    presenter::set_fill_tool();
    break;

  case cfg::ShortcutKey::TOOL_SELECT:
    presenter::set_select_tool();
    break;

  case cfg::ShortcutKey::ACTION_NEW:
    presenter::new_file();
    break;

  case cfg::ShortcutKey::ACTION_SAVE:
    presenter::save_file();
    break;

  case cfg::ShortcutKey::ACTION_OPEN:
    presenter::open_file();
    break;

  case cfg::ShortcutKey::ACTION_UNDO:
    presenter::undo_action();
    break;

  case cfg::ShortcutKey::ACTION_REDO:
    presenter::redo_action();
    break;

  case cfg::ShortcutKey::ACTION_UNSELECT:
    handle_unselect();
    break;

  case cfg::ShortcutKey::TIMELINE_NEXT_FRAME:
    if (model_.frame_index < model_.anim.get_frame_count() - 1U) {
      presenter::set_active_image(model_.frame_index + 1U, model_.layer_index);
    }
    break;

  case cfg::ShortcutKey::TIMELINE_PREV_FRAME:
    if (model_.frame_index > 0U) {
      presenter::set_active_image(model_.frame_index - 1U, model_.layer_index);
    }
    break;

  default:
    // Do nothing
    break;
  }
}

void presenter::new_file() noexcept {
  TRY_ABORT(
      view_.push_modal(view::modal::Id::NEW_FILE_MODAL),
      "Could not add new file modal"
  );
}

void presenter::set_active_image(u32 frame_index, i32 layer_index) noexcept {
  if (model_.frame_index == frame_index && model_.layer_index == layer_index) {
    return;
  }

  logger::info("Selected (Frame %u, Layer %d)", frame_index, layer_index);

  model_.frame_index = frame_index;
  model_.layer_index = layer_index;
  auto id = model_.anim.get_image_id(frame_index, layer_index);
  if (id != model_.img_id) {
    if (model_.img_id > 0U) {
      TRY_ABORT(
          model_.anim.write_pixels_to_disk(model_.img_id),
          "Could not write to disk"
      );
    }
    model_.img_id = id;
  }

  if (model_.img_id > 0U) {
    model_.img = *TRY_ABORT_RET(
        model_.anim.get_image(model_.img_id), "Could not read anim"
    );
  }

  view_.set_active_on_timeline(frame_index, layer_index);
}

void presenter::set_selected_layer(u32 selected_layer) noexcept {
  model_.selected_layer = selected_layer;
}

void presenter::set_selected_frame(usize selected_frame) noexcept {
  model_.selected_frame = selected_frame;
}

void presenter::set_cursor_position(fvec mouse) noexcept {
  model_.curr_pos.x =
      static_cast<i32>(std::floor((mouse.x - model_.rect.x) / model_.scale));
  model_.curr_pos.y =
      static_cast<i32>(std::floor((mouse.y - model_.rect.y) / model_.scale));
  view_.set_cursor_canvas_pos(model_.curr_pos);
}

void presenter::debug_callback() noexcept {
  logger::info("Debug callback UwU");
}

#ifndef NDEBUG

void presenter::debug_pixels() noexcept {
  if (!logger::lock(logger::DEBUG_LVL, "Printing pixels")) {
    return;
  }

  // NOTE: Only works for 4 bytes for now
  usize i = 0;
  for (usize y = 0; y < model_.anim.get_height(); ++y) {
    for (usize x = 0; x < model_.anim.get_width(); ++x) {
      logger::print(color::to_hex_string(*(rgba8*)&model_.pixels[i]).c_str());
      i += 4;
    }
    logger::print("\n");
  }
  logger::unlock();
}

#endif

