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
#include "core/history/caretaker.hpp"
#include "core/history/snapshot.hpp"
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
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

// NOTE: Can be changed depending on the gui to be used
#include "view/modal.hpp"
#include "view/sdl3/manager.hpp"
using View = view::sdl3::Manager;

// Tools
namespace presenter {

inline tool::Pencil pencil{};
inline tool::Eraser eraser{};
inline tool::Line line{};
inline tool::Fill fill{};
inline tool::Select select{};

inline tool::Pan pan{};
inline tool::Zoom zoom{};

inline cfg::Shortcut shortcut{};

// History
inline history::Caretaker caretaker{};

inline Model model{};
inline View view{};

} // namespace presenter

void presenter::init() noexcept {
  // TODO: make this changeable while app is running
  cfg::locale::load_locale(cfg::locale::Locale::ENGLISH);
  shortcut.load_config("keys.cfg");

  view.init();
}

void presenter::run() noexcept {
  view.run();

  view.~View();
  model.~Model();
}

// === Events === //

void presenter::window_resized() noexcept {
  model.bounds = view.get_draw_rect();

  // Clamping
  model.rect.x = std::clamp(
      model.rect.x, model.bounds.x - model.rect.w + 32.0F,
      model.bounds.x + model.bounds.w - 32.0F
  );
  model.rect.y = std::clamp(
      model.rect.y, model.bounds.y - model.rect.h + 32.0F,
      model.bounds.y + model.bounds.h - 32.0F
  );
  view.set_canvas_rect(model.rect);
}

inline void update_canvas_texture() noexcept {
  using namespace presenter;
  auto id = model.anim.get_layer_id(model.frame_id, model.layer_index);
  presenter::view.get_curr_texture().set_pixels(
      (rgba8*)model.anim.get_layer(id).get_ptr(), model.anim.get_size()
  );
}

inline void handle_unselect() noexcept {
  logger::info("Unselect");
  std::fill(
      presenter::model.select_mask.begin(), presenter::model.select_mask.end(),
      true
  );

  presenter::view.get_select1_texture().clear(presenter::model.anim.get_height()
  );
  presenter::view.get_select2_texture().clear(presenter::model.anim.get_height()
  );
}

void presenter::key_down_event(
    input::Keycode keycode, input::KeyMod key_mod
) noexcept {
  using namespace input;
  switch (shortcut.get_shortcut_key(
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

  case cfg::ShortcutKey::ACTION_UNDO:
    if (!caretaker.can_undo())
      break;
    logger::info("Undo");
    caretaker.undo().restore(model);
    update_canvas_texture();
    break;

  case cfg::ShortcutKey::ACTION_REDO:
    if (!caretaker.can_redo())
      break;
    logger::info("Redo");
    caretaker.redo().restore(model);
    update_canvas_texture();
    break;

  case cfg::ShortcutKey::ACTION_UNSELECT:
    handle_unselect();
    break;

  default:
    // Do nothing
    break;
  }
}

void handle_flags(u32 flags) {
  using namespace event;
  using namespace presenter;
  if (flags & Flag::SNAPSHOT) {
    history::Snapshot snapshot{};
    snapshot.snap(model);
    caretaker.push_snapshot(std::move(snapshot));
  }
}

inline void handle_canvas_mouse_middle(const event::Input& evt) noexcept {
  using namespace presenter;
  // NOTE: Check if there any available things other than panning
  u32 flags = 0U;
  flags = pan.execute(model, evt);
  presenter::view.set_canvas_rect(model.rect);
  handle_flags(flags);
}

inline void canvas_mouse_scroll_event(const event::Input& evt) noexcept {
  using namespace presenter;
  u32 flags = 0U;
  flags = zoom.execute(model, evt);
  presenter::view.set_canvas_rect(model.rect);
  handle_flags(flags);
}

void presenter::canvas_mouse_event(const event::Input& evt) noexcept {
  if (evt.mouse.wheel.x != 0 || evt.mouse.wheel.y != 0) {
    canvas_mouse_scroll_event(evt);
    // Continue with other mouse events
  }

  if (evt.mouse.middle.state != input::MouseState::NONE) {
    handle_canvas_mouse_middle(evt);
    return;
  }

  model.curr_pos = {
      .x = (i32)std::floor((evt.mouse.pos.x - model.rect.x) / model.scale),
      .y = (i32)std::floor((evt.mouse.pos.y - model.rect.y) / model.scale),
  };
  view.set_cursor_canvas_pos(model.curr_pos);

  using namespace tool;
  u32 flags = 0U;
  switch (model.tool) {
  case Type::PENCIL:
    flags = pencil.execute(model, evt);
    break;

  case Type::ERASER:
    flags = eraser.execute(model, evt);
    break;

  case Type::LINE:
    flags = line.execute(model, evt);
    break;

  case Type::FILL:
    flags = fill.execute(model, evt);
    break;

  case Type::SELECT:
    flags = select.execute(model, evt);
    break;

  default:
    // Do nothing
    break;
  }

  handle_flags(flags);
  model.prev_pos = model.curr_pos;
}

// === Tool Buttons === //
void presenter::set_pencil_tool() noexcept {
  logger::info("Pencil Tool");
  model.tool = tool::Type::PENCIL;
  model.tex1 = &view.get_curr_texture();
  model.tex2 = &view.get_empty_texture();
}

void presenter::set_eraser_tool() noexcept {
  logger::info("Eraser Tool");
  model.tool = tool::Type::ERASER;
  model.tex1 = &view.get_curr_texture();
  model.tex2 = &view.get_empty_texture();
}

void presenter::set_line_tool() noexcept {
  logger::info("Line Tool");
  model.tool = tool::Type::LINE;
  model.tex1 = &view.get_curr_texture();
  model.tex2 = &view.get_empty_texture();
}

void presenter::set_fill_tool() noexcept {
  logger::info("Fill Tool");
  model.tool = tool::Type::FILL;
  model.tex1 = &view.get_curr_texture();
  model.tex2 = &view.get_empty_texture();
}

void presenter::set_select_tool() noexcept {
  logger::info("Select Tool");
  model.tool = tool::Type::SELECT;
  model.tex1 = &view.get_select1_texture();
  model.tex2 = &view.get_select2_texture();
}

void presenter::close_modals() noexcept {
  view.clear_modals();
}

void presenter::new_file_clicked() noexcept {
  view.push_modal(view::modal::Id::NEW_FILE_MODAL);
}

void presenter::create_anim() noexcept {
  // TODO: Prototype
  if (model.anim) {
    return;
  }

  auto* data = (view::modal::NewFileData*)view.get_modal_data(
      view::modal::Id::NEW_FILE_MODAL
  );
  if (data == nullptr) {
    logger::fatal("Data is null");
    std::abort();
  }

  auto size = data->size;
  delete data; // NOLINT
  view.clear_modals();

  if (size.x <= 0 || size.y <= 0) {
    return;
  }

  model.anim.init(size, draw::RGBA8);
  model.frame_id = 1U;
  model.layer_index = 0;
  auto id = model.anim.get_layer_id(model.frame_id, model.layer_index);
  model.layer = model.anim.get_layer(id);
  model.select_mask.resize(size.x * size.y); // NOLINT
  std::fill(model.select_mask.begin(), model.select_mask.end(), true);

  model.tex1 = &view.get_curr_texture();
  model.tex2 = &view.get_empty_texture();
  model.bounds = view.get_draw_rect();

  model.rect = {
      .x = model.bounds.x + 10.0F,
      .y = model.bounds.y + 10.0F,
      .w = size.x * model.scale,
      .h = size.y * model.scale};
  view.set_canvas_rect(model.rect);
  view.set_draw_size(size);

  history::Snapshot snapshot{};
  snapshot.snap(model);
  caretaker.push_snapshot(std::move(snapshot));
}

void presenter::debug_callback() noexcept {
  logger::info("Debug callback UwU");
}

