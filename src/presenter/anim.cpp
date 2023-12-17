/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-23
 *==========================*/

/**
 * Presenter functions for animation mutations
 **/

#include "./presenter.hpp"
#include "core/file/pxl.hpp"
#include "core/history/action.hpp"
#include "core/history/caretaker.hpp"
#include "core/logger/logger.hpp"
#include "model/model.hpp"
#include <algorithm>

void presenter::create_anim() noexcept {
  // TODO: Prototype
  if (model_.anim) {
    return;
  }

  auto* data = static_cast<view::modal::NewFileData*>(
      view_.get_modal_data(view::modal::Id::NEW_FILE_MODAL)
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

  model_.frame_index = 0U;
  model_.layer_index = 0U;
  model_.img_id = 1U;
  model_.img = *TRY_ABORT_RET(model_.anim.get_image(1U), "Could not read anim");
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

  // View Update
  TRY_ABORT(view_.set_anim(&model_.anim), "Could not set anim");
  view_.set_canvas_rect(model_.rect);
  view_.set_draw_size(size);
  view_.set_active_on_timeline(0U, 0U);

  TRY_ABORT(
      view_.insert_layer(0U, model_.anim.get_layer_info(0U)),
      "Could not update view"
  );
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
    view_.update_curr_texture(model_.img_id, show);
  } else if (layer_index < model_.layer_index) {
    view_.update_bot_texture(model_.frame_index, model_.layer_index);
  } else {
    view_.update_top_texture(model_.frame_index, model_.layer_index);
  }
}

void presenter::insert_at_selected_layer() noexcept {
  if (!model_.anim) {
    return;
  }

  ++model_.selected_layer; // Adjust
  assert(model_.selected_layer <= model_.anim.get_layer_count());

  logger::info("Insert layer " USIZE_FMT, model_.selected_layer);

  history::Action action{history::ActionType::INSERT_LAYER};
  action.insert_layer.prev_layer_index = model_.layer_index;
  action.insert_layer.layer_index = model_.selected_layer;
  caretaker_.prepare_push_action();
  caretaker_.push_action(std::move(action));

  model_.layer_index = model_.selected_layer;
  TRY_ABORT(
      model_.anim.insert_layer(model_.layer_index), "Could not update anim"
  );
  model_.img_id = 0U;

  TRY_IGNORE(pxl_.try_auto_save(model_.anim), "Could not auto save");

  TRY_ABORT(
      view_.insert_layer(
          model_.layer_index, model_.anim.get_layer_info(model_.layer_index)
      ),
      "Could not update view"
  );
  view_.set_active_on_timeline(model_.frame_index, model_.layer_index);
}

void presenter::insert_at_selected_frame() noexcept {
  if (!model_.anim) {
    return;
  }

  logger::info("Insert frame " USIZE_FMT, model_.selected_frame);

  history::Action action{history::ActionType::INSERT_FRAME};
  action.insert_frame.prev_frame_index = model_.frame_index;
  action.insert_frame.frame_index = model_.selected_frame;
  caretaker_.prepare_push_action();
  caretaker_.push_action(std::move(action));

  TRY_ABORT(
      model_.anim.insert_frame(model_.selected_frame), "Could not update anim"
  );

  model_.frame_index = model_.selected_frame;
  model_.img_id = 0U;

  view_.set_active_on_timeline(model_.selected_frame, model_.layer_index);
  view_.set_frame_range(0U, model_.anim.get_frame_count() - 1U);
}

void presenter::remove_at_selected_layer() noexcept {
  if (!model_.anim || model_.anim.get_layer_count() == 1U) {
    return;
  }

  if (model_.selected_layer == USIZE_MAX) {
    model_.selected_layer = 0U;
  }

  logger::info("Remove layer " USIZE_FMT, model_.selected_layer);

  caretaker_.prepare_push_action();
  history::Action action{history::ActionType::REMOVE_LAYER};
  action.remove_layer.init(model_.anim, model_.selected_layer, caretaker_);
  caretaker_.push_action(std::move(action));

  TRY_ABORT(
      model_.anim.remove_layer(model_.selected_layer), "Could not update anim"
  );

  if (model_.selected_layer < model_.layer_index) {
    --model_.layer_index;
  }

  auto id = model_.anim.get_image_id(model_.frame_index, model_.layer_index);
  if (model_.img_id != id) {
    model_.img_id = id;
    if (id > 0U) {
      model_.img =
          *TRY_ABORT_RET(model_.anim.get_image(id), "Could not read anim");
    }
  }

  // Update view
  view_.remove_layer(model_.selected_layer);
  view_.set_active_on_timeline(model_.frame_index, model_.layer_index);
}

void presenter::remove_at_selected_frame() noexcept {
  if (!model_.anim || model_.anim.get_frame_count() == 1U) {
    return;
  }

  model_.selected_frame =
      std::min(model_.selected_frame, model_.anim.get_frame_count() - 1U);

  caretaker_.prepare_push_action();
  history::Action action{history::ActionType::REMOVE_FRAME};
  action.remove_frame.init(model_.anim, model_.selected_frame, caretaker_);
  caretaker_.push_action(std::move(action));

  TRY_ABORT(
      model_.anim.remove_frame(model_.selected_frame), "Could not update anim"
  );

  if (model_.selected_frame <= model_.frame_index) {
    --model_.frame_index;
  }

  auto id = model_.anim.get_image_id(model_.frame_index, model_.layer_index);
  if (model_.img_id != id) {
    model_.img_id = id;
    if (id > 0U) {
      model_.img =
          *TRY_ABORT_RET(model_.anim.get_image(id), "Could not read anim");
    }
  }

  // Update view
  view_.set_frame_range(0U, model_.anim.get_frame_count() - 1U);
  view_.set_active_on_timeline(model_.frame_index, model_.layer_index);
}

void presenter::undo_action() noexcept {
  if (!caretaker_.undo(model_))
    return;

  logger::info("Undo");
  update_view();
}

void presenter::redo_action() noexcept {
  if (!caretaker_.redo(model_))
    return;

  logger::info("Redo");
  update_view();
}

