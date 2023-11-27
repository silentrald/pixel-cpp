/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-23
 *==========================*/

/**
 * Presenter functions that only updates view elements
 **/

#include "./presenter.hpp"
#include "model/model.hpp"

void presenter::update_canvas_textures() noexcept {
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

void presenter::update_view() noexcept {
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

void presenter::close_modals() noexcept {
  view_.clear_modals();
}