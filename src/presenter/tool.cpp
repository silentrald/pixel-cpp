/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-23
 *==========================*/

/**
 * Presenter functions that handles tools.
 **/

#include "./presenter.hpp"
#include "core/file/pxl.hpp"
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
#include <cmath>

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

inline void handle_canvas_mouse_middle(const event::Input& evt) noexcept {
  using namespace presenter;
  // NOTE: Check if there any available things other than panning
  static_cast<void>(pan_.execute(model_, evt));
  view_.set_canvas_rect(model_.rect);
}

void presenter::canvas_mouse_scroll_event(const event::Input& evt) noexcept {
  if (!model_.anim) {
    return;
  }

  static_cast<void>(zoom_.execute(model_, evt));
  view_.set_canvas_rect(model_.rect);
}

inline void handle_flags(u32 flags) {
  using namespace event;
  using namespace presenter;
  if (flags & Flag::SNAPSHOT) {
    model_.is_editing_image = false;

    caretaker_.prepare_push_action();

    history::Action action =
        caretaker_.create_image_action(model_.anim.get_image_bytes_size());
    action.type = model_.is_added_image ? history::ActionType::ADD_IMAGE
                                        : history::ActionType::EDIT_IMAGE;
    action.image.frame_index = model_.frame_index;
    action.image.layer_index = model_.layer_index;

    model_.is_added_image = false;

    std::memcpy(
        action.image.prev_pixels, model_.pixels.get_data(),
        model_.anim.get_image_bytes_size()
    );
    std::memcpy(
        action.image.pixels, model_.img.get_pixels(),
        model_.anim.get_image_bytes_size()
    );

    caretaker_.push_action(std::move(action));

    view_.update_curr_texture(
        model_.img_id, model_.anim.is_layer_visible(model_.layer_index)
    );
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

  if (!evt.is_mouse_state(input::MouseState::NONE) &&
      !model_.anim.is_layer_visible(model_.layer_index)) {
    logger::info("Layer is hidden");
    return;
  }

  if (evt.mouse.middle != input::MouseState::NONE) {
    handle_canvas_mouse_middle(evt);
    return;
  }

  if (model_.img_id == 0U) {
    // Create a image in the db
    model_.img_id = *TRY_ABORT_RET(
        model_.anim.create_image(model_.frame_index, model_.layer_index),
        "Could not create image"
    );
    model_.img = model_.anim.get_image_fast(model_.img_id);
    model_.is_added_image = true;
  }

  if (evt.mouse.left == input::MouseState::DOWN ||
      evt.mouse.right == input::MouseState::DOWN) {
    model_.is_editing_image = true;
    std::memcpy(
        model_.pixels.get_data(), model_.img.get_pixels(),
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

