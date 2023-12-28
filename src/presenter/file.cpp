/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-23
 *==========================*/

/**
 * Presenter functions for file reads/writes. Mostly done by save/import/export.
 **/

#include "./presenter.hpp"
#include "core/file/png.hpp"
#include "core/file/pxl.hpp"
#include "model/model.hpp"

void presenter::save_file() noexcept {
  if (!model_.anim) {
    return;
  }

  // TODO: If no file name set, ask the user for a file to save to

  logger::info("Saving");
  if (model_.img_id > 0U) {
    TRY_ABORT(
        model_.anim.write_pixels_to_disk(model_.img_id),
        "Could not write to disk"
    );
  }
  TRY_ABORT(pxl_.save(model_.anim, "save.pxl"), "Could not save anim");

  logger::info("Save successful");
}

void presenter::open_file() noexcept {
  if (model_.anim) {
    return;
  }

  logger::info("Open");
  model_.anim = *TRY_ABORT_RET(pxl_.load("save.pxl"), "Could not load anim");

  model_.frame_index = 0U;
  model_.layer_index = model_.anim.get_layer_count() - 1U;
  model_.img_id = model_.anim.get_image_id(0U, 0U);
  model_.img = *TRY_ABORT_RET(
      model_.anim.get_image(model_.img_id), "Could not read anim"
  );
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

  TRY_ABORT(
      model_.pixels.resize(model_.anim.get_image_bytes_size()),
      "Could not create cache"
  );

  // Update view
  TRY_ABORT(view_.set_anim(&model_.anim), "Could not set anim");

  update_view();

  logger::info("Open successful");
}

void presenter::export_to_png() noexcept {
  if (!model_.anim) {
    return;
  }

  logger::info("Export to \"%s\"", "sample.png");
  png_.export_frame(model_.anim, model_.frame_index, "sample.png");
  logger::info("Export successfully");
}

