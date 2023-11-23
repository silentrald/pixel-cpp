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
#include "core/file/png.hpp"
#include "core/file/pxl.hpp"
#include "core/history/action.hpp"
#include "core/history/caretaker.hpp"
#include "core/logger/logger.hpp"
#include "core/tool/enum.hpp"
#include "core/tool/eraser.hpp"
#include "core/tool/fill.hpp"
#include "core/tool/line.hpp"
#include "core/tool/pan.hpp"
#include "core/tool/pencil.hpp"
#include "core/tool/select.hpp"
#include "core/tool/zoom.hpp"
#include "model/model.hpp"
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
      shortcut_.load_config("keys.cfg"),
      "Could not load initial shortcut key mapping"
  );

  TRY_ABORT(caretaker_.init(50), "Could not init caretaker");

  pxl_.set_auto_save(50U);
  view_.init();
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

void update_canvas_textures() noexcept {
  std::memset(model_.pixels.get_data(), 0, model_.pixels.get_size());

  // Bot
  if (model_.layer_index > 0) {
    model_.anim.get_flatten(
        model_.frame_id, 0, model_.layer_index - 1, model_.pixels
    );
  }
  view_.get_bot_texture().set_pixels(
      (rgba8*)model_.pixels.get_data(), model_.anim.get_size()
  );

  // Clear
  std::memset(model_.pixels.get_data(), 0, model_.anim.get_image_bytes_size());

  // Top
  if (model_.layer_index < model_.anim.get_layer_count() - 1) {
    model_.anim.get_flatten(
        model_.frame_id, model_.layer_index + 1,
        model_.anim.get_layer_count() - 1, model_.pixels
    );
  }
  view_.get_top_texture().set_pixels(
      (rgba8*)model_.pixels.get_data(), model_.anim.get_size()
  );

  if (model_.anim.is_layer_visible(model_.layer_index)) {
    view_.get_curr_texture().set_pixels(
        (rgba8*)model_.img.get_pixels(), model_.anim.get_size()
    );
  } else {
    view_.get_curr_texture().clear(model_.anim.get_height());
  }
}

void update_view() noexcept {
  // Update canvas
  update_canvas_textures();

  // Update the timeline
  view_.clear_layers();
  for (i32 i = model_.anim.get_layer_count() - 1; i >= 0; --i) {
    auto layer_info = model_.anim.get_layer_info(i);
    TRY_ABORT(
        view_.insert_layer(0, model_.anim.get_layer_info(i)),
        "Could not update view"
    );
  }

  view_.set_selected_on_timeline(model_.frame_id, model_.layer_index);
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

  case cfg::ShortcutKey::ACTION_SAVE:
    presenter::save_file();
    break;

  case cfg::ShortcutKey::ACTION_OPEN:
    presenter::open_file();
    break;

  case cfg::ShortcutKey::ACTION_UNDO:
    if (!caretaker_.undo(model_))
      break;

    logger::info("Undo");
    update_view();
    break;

  case cfg::ShortcutKey::ACTION_REDO:
    if (!caretaker_.redo(model_))
      break;

    logger::info("Redo");
    update_view();
    break;

  case cfg::ShortcutKey::ACTION_UNSELECT:
    handle_unselect();
    break;

  default:
    // Do nothing
    break;
  }
}

inline void handle_canvas_mouse_middle(const event::Input& evt) noexcept {
  using namespace presenter;
  // NOTE: Check if there any available things other than panning
  (void)pan_.execute(model_, evt);
  view_.set_canvas_rect(model_.rect);
}

inline void canvas_mouse_scroll_event(const event::Input& evt) noexcept {
  using namespace presenter;
  (void)zoom_.execute(model_, evt);
  view_.set_canvas_rect(model_.rect);
}

inline void handle_flags(u32 flags) {
  using namespace event;
  using namespace presenter;
  if (flags & Flag::SNAPSHOT) {
    model_.is_editing_image = false;

    caretaker_.prepare_push_action();

    history::Action action =
        caretaker_.create_edit_image_action(model_.anim.get_image_bytes_size());
    std::memcpy(
        action.edit_image.prev_pixels, model_.orig_pixels.get_data(),
        model_.anim.get_image_bytes_size()
    );
    std::memcpy(
        action.edit_image.pixels, model_.img.get_pixels(),
        model_.anim.get_image_bytes_size()
    );

    caretaker_.push_action(std::move(action));

    if (pxl_.will_auto_save()) {
      logger::info("Auto-save");
      TRY_ABORT(
          model_.anim.write_pixels_to_disk(model_.img_id),
          "Could not write to disk"
      );
      TRY_ABORT(pxl_.force_auto_save(model_.anim), "Could not save");
    }
  }
}

void presenter::canvas_mouse_event(const event::Input& evt) noexcept {
  if (!model_.anim) {
    return;
  }

  if (evt.mouse.wheel.x != 0 || evt.mouse.wheel.y != 0) {
    canvas_mouse_scroll_event(evt);
    // Continue with other mouse events
  }

  if (!evt.is_mouse_state(input::MouseState::NONE) &&
      !model_.anim.is_layer_visible(model_.layer_index)) {
    logger::info("Layer is hidden");
    return;
  }

  if (evt.mouse.middle.state != input::MouseState::NONE) {
    handle_canvas_mouse_middle(evt);
    return;
  }

  model_.curr_pos = {
      .x = (i32)std::floor((evt.mouse.pos.x - model_.rect.x) / model_.scale),
      .y = (i32)std::floor((evt.mouse.pos.y - model_.rect.y) / model_.scale),
  };
  view_.set_cursor_canvas_pos(model_.curr_pos);

  if (evt.mouse.left.state == input::MouseState::DOWN ||
      evt.mouse.right.state == input::MouseState::DOWN) {
    model_.is_editing_image = true;
    std::memcpy(
        model_.orig_pixels.get_data(), model_.img.get_pixels(),
        model_.anim.get_image_bytes_size()
    );
  }

  u32 flags = 0U;
  switch (model_.tool) {
  case tool::Type::PENCIL:
    flags = pencil_.execute(model_, evt);
    break;

  case tool::Type::ERASER:
    flags = eraser_.execute(model_, evt);
    break;

  case tool::Type::LINE:
    flags = line_.execute(model_, evt);
    break;

  case tool::Type::FILL:
    flags = fill_.execute(model_, evt);
    break;

  case tool::Type::SELECT:
    flags = select_.execute(model_, evt);
    break;

  default:
    // Do nothing
    break;
  }

  handle_flags(flags);
  model_.prev_pos = model_.curr_pos;
}

// === Tool Buttons === //
void presenter::set_pencil_tool() noexcept {
  logger::info("Pencil Tool");
  model_.tool = tool::Type::PENCIL;
  model_.tex1 = &view_.get_curr_texture();
  model_.tex2 = &view_.get_empty_texture();
}

void presenter::set_eraser_tool() noexcept {
  logger::info("Eraser Tool");
  model_.tool = tool::Type::ERASER;
  model_.tex1 = &view_.get_curr_texture();
  model_.tex2 = &view_.get_empty_texture();
}

void presenter::set_line_tool() noexcept {
  logger::info("Line Tool");
  model_.tool = tool::Type::LINE;
  model_.tex1 = &view_.get_curr_texture();
  model_.tex2 = &view_.get_empty_texture();
}

void presenter::set_fill_tool() noexcept {
  logger::info("Fill Tool");
  model_.tool = tool::Type::FILL;
  model_.tex1 = &view_.get_curr_texture();
  model_.tex2 = &view_.get_empty_texture();
}

void presenter::set_select_tool() noexcept {
  logger::info("Select Tool");
  model_.tool = tool::Type::SELECT;
  model_.tex1 = &view_.get_select1_texture();
  model_.tex2 = &view_.get_select2_texture();
}

void presenter::close_modals() noexcept {
  view_.clear_modals();
}

void presenter::new_file_clicked() noexcept {
  TRY_ABORT(
      view_.push_modal(view::modal::Id::NEW_FILE_MODAL),
      "Could not add new file modal"
  );
}

void presenter::create_anim() noexcept {
  // TODO: Prototype
  if (model_.anim) {
    return;
  }

  auto* data = (view::modal::NewFileData*)view_.get_modal_data(
      view::modal::Id::NEW_FILE_MODAL
  );
  if (data == nullptr) {
    logger::fatal("Data is null");
    std::abort();
  }

  auto size = data->size;
  delete data; // NOLINT
  view_.clear_modals();

  if (size.x <= 0 || size.y <= 0) {
    return;
  }

  // Model Update
  TRY_ABORT(model_.anim.init(size, draw::RGBA8), "Could not create anim");

  model_.frame_id = 1U;
  model_.layer_index = 0U;
  model_.img_id = 1U;
  auto id = model_.anim.get_image_id(model_.frame_id, model_.layer_index);
  model_.img = *TRY_ABORT_RET(model_.anim.get_image(id), "Could not read anim");
  model_.select_mask.resize(size.x * size.y); // NOLINT
  std::fill(model_.select_mask.begin(), model_.select_mask.end(), true);

  model_.tex1 = &view_.get_curr_texture();
  model_.tex2 = &view_.get_empty_texture();
  model_.bounds = view_.get_draw_rect();

  model_.rect = {
      .x = model_.bounds.x + 10.0F,
      .y = model_.bounds.y + 10.0F,
      .w = size.x * model_.scale,
      .h = size.y * model_.scale};

  TRY_ABORT(
      model_.pixels.resize(model_.anim.get_image_bytes_size()),
      "Could not create cache"
  );
  TRY_ABORT(
      model_.orig_pixels.resize(model_.anim.get_image_bytes_size()),
      "Could not create cache"
  );

  // View Update
  view_.set_canvas_rect(model_.rect);
  view_.set_draw_size(size);

  TRY_ABORT(
      view_.insert_layer(0U, model_.anim.get_layer_info(0U)),
      "Could not update view"
  );
}

void presenter::set_selected(u32 frame_id, i32 layer_index) noexcept {
  logger::info("Selected (Frame %u, Layer %d)", frame_id, layer_index);

  history::Action action{history::ActionType::CHANGE_SELECTION};
  action.change_selection.prev_frame_id = model_.frame_id;
  action.change_selection.prev_layer_index = model_.layer_index;
  action.change_selection.frame_id = frame_id;
  action.change_selection.layer_index = layer_index;
  caretaker_.prepare_push_action();
  caretaker_.push_action(std::move(action));

  model_.frame_id = frame_id;
  model_.layer_index = layer_index;
  auto id = model_.anim.get_image_id(frame_id, layer_index);
  if (id != model_.img_id) {
    TRY_ABORT(
        model_.anim.write_pixels_to_disk(model_.img_id),
        "Could not write to disk"
    );
    model_.img_id = id;
  }
  model_.img = *TRY_ABORT_RET(
      model_.anim.get_image(model_.img_id), "Could not read anim"
  );

  update_canvas_textures();

  view_.set_selected_on_timeline(frame_id, layer_index);
}

void presenter::toggle_visibility(i32 layer_index) noexcept {
  logger::info("Toggle visibility (Layer %d)", layer_index);

  bool show = model_.anim.toggle_layer_visibility(layer_index);

  history::Action action{history::ActionType::SET_VISIBILITY};
  action.set_visibility.layer_index = layer_index;
  action.set_visibility.visibility = show;
  caretaker_.prepare_push_action();
  caretaker_.push_action(std::move(action));

  view_.set_layer_visible(layer_index, show);

  if (layer_index == model_.layer_index) {
    if (show) {
      auto layer = *TRY_ABORT_RET(
          model_.anim.get_image(model_.img_id), "Could not read anim"
      );
      view_.get_curr_texture().set_pixels(
          (rgba8*)layer.get_pixels(), model_.anim.get_size()
      );
    } else {
      view_.get_curr_texture().clear(model_.anim.get_height());
    }
    return;
  }

  memset(model_.pixels.get_data(), 0, model_.pixels.get_size());

  // Bot
  if (layer_index < model_.layer_index) {
    model_.anim.get_flatten(
        model_.frame_id, 0, model_.layer_index - 1, model_.pixels
    );
    view_.get_bot_texture().set_pixels(
        (rgba8*)model_.pixels.get_data(), model_.anim.get_size()
    );
    return;
  }

  // Top
  model_.anim.get_flatten(
      model_.frame_id, model_.layer_index + 1,
      model_.anim.get_layer_count() - 1, model_.pixels
  );
  view_.get_top_texture().set_pixels(
      (rgba8*)model_.pixels.get_data(), model_.anim.get_size()
  );
}

void presenter::insert_layer(i32 layer_index) noexcept {
  if (!model_.anim) {
    return;
  }

  history::Action action{history::ActionType::INSERT_LAYER};
  action.insert_layer.prev_layer_index = model_.layer_index;
  action.insert_layer.layer_index = layer_index;
  caretaker_.prepare_push_action();
  caretaker_.push_action(std::move(action));

  usize id = *TRY_ABORT_RET(
      model_.anim.insert_layer(layer_index), "Could not update anim"
  );
  model_.img_id = id;
  model_.img = *model_.anim.get_image(id); // Should throw an error

  TRY_IGNORE(pxl_.try_auto_save(model_.anim), "Could not auto save");

  TRY_ABORT(
      view_.insert_layer(layer_index, model_.anim.get_layer_info(layer_index)),
      "Could not update view"
  );
  update_canvas_textures();
  view_.set_selected_on_timeline(model_.frame_id, layer_index);
}

void presenter::push_back_layer() noexcept {
  if (!model_.anim) {
    return;
  }
  logger::info("New layer");

  history::Action action{history::ActionType::INSERT_LAYER};
  action.insert_layer.prev_layer_index = model_.layer_index;
  action.insert_layer.layer_index = model_.anim.get_layer_count();
  caretaker_.prepare_push_action();
  caretaker_.push_action(std::move(action));

  model_.layer_index = model_.anim.get_layer_count();
  usize id = *TRY_ABORT_RET(
      model_.anim.insert_layer(model_.layer_index), "Could not update anim"
  );
  model_.img_id = id;
  model_.img = *model_.anim.get_image(id); // Shouldn't throw an error

  TRY_IGNORE(pxl_.try_auto_save(model_.anim), "Could not auto save");

  TRY_ABORT(
      view_.insert_layer(
          model_.layer_index, model_.anim.get_layer_info(model_.layer_index)
      ),
      "Could not update view"
  );
  update_canvas_textures();
  view_.set_selected_on_timeline(model_.frame_id, model_.layer_index);
}

void presenter::save_file() noexcept {
  if (!model_.anim) {
    return;
  }

  // TODO: If no file name set, ask the user for a file to save to

  logger::info("Saving");
  TRY_ABORT(
      model_.anim.write_pixels_to_disk(model_.img_id), "Could not write to disk"
  );
  TRY_ABORT(pxl_.save(model_.anim, "save.pxl"), "Could not save anim");
  logger::info("Save successful");
}

void presenter::open_file() noexcept {
  if (model_.anim) {
    return;
  }

  logger::info("Open");
  model_.anim = *TRY_ABORT_RET(pxl_.load("save.pxl"), "Could not load anim");

  model_.frame_id = 1U;
  model_.layer_index = 0U;
  model_.img_id = 1U;
  auto id = model_.anim.get_image_id(1U, 0U);
  model_.img = *TRY_ABORT_RET(model_.anim.get_image(id), "Could not read anim");
  model_.select_mask.resize(
      // NOLINTNEXTLINE
      model_.anim.get_width() * model_.anim.get_height()
  );
  std::fill(model_.select_mask.begin(), model_.select_mask.end(), true);

  model_.tex1 = &view_.get_curr_texture();
  model_.tex2 = &view_.get_empty_texture();
  model_.bounds = view_.get_draw_rect();

  model_.rect = {
      .x = model_.bounds.x + 10.0F,
      .y = model_.bounds.y + 10.0F,
      .w = model_.anim.get_width() * model_.scale,
      .h = model_.anim.get_height() * model_.scale};
  view_.set_canvas_rect(model_.rect);
  view_.set_draw_size(model_.anim.get_size());

  update_view();

  logger::info("Open successful");
}

void presenter::export_to_png() noexcept {
  if (!model_.anim) {
    return;
  }

  logger::info("Export to \"%s\"", "sample.png");
  png_.export_frame(model_.anim, model_.frame_id, "sample.png");
  logger::info("Export successfully");
}

void presenter::debug_callback() noexcept {
  logger::info("Debug callback UwU");
}

