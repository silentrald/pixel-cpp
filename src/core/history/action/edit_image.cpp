/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#include "./edit_image.hpp"
#include "core/logger/logger.hpp"

namespace history {

void EditImage::undo(Model& model) const noexcept {
  if (this->new_image) {
    TRY_ABORT(
        model.anim.remove_image(this->frame_index, this->layer_index),
        "Anim remove image"
    );
    model.frame_index = this->frame_index;
    model.layer_index = this->layer_index;
    model.img_id = 0U;
    return;
  }

  // Try to get the img_id if it matches, else update the model.img
  auto id = model.anim.get_image_id(this->frame_index, this->layer_index);
  if (model.img_id != id) {
    model.img_id = id;
    model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Anim read");
  }

  model.img.set_pixels(this->prev_pixels, model.anim.get_image_bytes_size());
  TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");

  model.layer_index = this->layer_index;
  model.frame_index = this->frame_index;
}

void EditImage::redo(Model& model) const noexcept {
  if (this->new_image) {
    auto id = *TRY_ABORT_RET(
        model.anim.create_image(this->frame_index, this->layer_index),
        "Anim write"
    );
    model.img_id = id;
    model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Anim read");

    model.img.set_pixels(this->pixels, model.anim.get_image_bytes_size());
    TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");

    model.layer_index = this->layer_index;
    model.frame_index = this->frame_index;
    return;
  }

  // Try to get the img_id if its correct
  auto id = model.anim.get_image_id(this->frame_index, this->layer_index);
  if (model.img_id != id) {
    model.img_id = id;
    model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Anim read");
  }

  model.img.set_pixels(this->pixels, model.anim.get_image_bytes_size());
  TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");

  model.layer_index = this->layer_index;
  model.frame_index = this->frame_index;
}

#ifndef NDEBUG

void EditImage::print(bool lock) const noexcept {
  LOCK_PRINT(logger::Level::DEBUG_LVL, "Edit Image", lock);

  // Info
  logger::print(
      "  Frame: " USIZE_FMT "\n  Layer: " USIZE_FMT "\n  New Image: %d\n",
      this->frame_index, this->layer_index, this->new_image
  );

  // NOTE: Data print if needed

  UNLOCK_PRINT(lock);
}

#endif

} // namespace history
