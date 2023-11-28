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
#include "model/model.hpp"

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

void presenter::add_at_selected_layer() noexcept {
  if (!model_.anim) {
    return;
  }

  assert(model_.selected_layer <= model_.anim.get_layer_count());

  history::Action action{history::ActionType::INSERT_LAYER};
  action.insert_layer.prev_layer_index = model_.layer_index;
  action.insert_layer.layer_index = model_.selected_layer;
  caretaker_.prepare_push_action();
  caretaker_.push_action(std::move(action));

  model_.layer_index = model_.selected_layer;
  usize id = *TRY_ABORT_RET(
      model_.anim.insert_layer(model_.layer_index), "Could not update anim"
  );
  model_.img_id = id;
  model_.img = *model_.anim.get_image(id); // Should throw an error

  TRY_IGNORE(pxl_.try_auto_save(model_.anim), "Could not auto save");

  TRY_ABORT(
      view_.insert_layer(
          model_.layer_index,
          model_.anim.get_layer_info(model_.layer_index)
      ),
      "Could not update view"
  );
  update_canvas_textures();
  view_.set_active_on_timeline(model_.frame_id, model_.layer_index);
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

