/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-17
 *===============================*/

#include "./remove_frame.hpp"
#include "core/history/caretaker.hpp"
#include "types.hpp"

namespace history {

struct RemoveFrameImage {
  usize layer_index = 0U;
  draw::data_type pixels[]; // NOLINT
};

void RemoveFrame::init(
    draw::Anim& anim, usize frame_index, Caretaker& caretaker
) noexcept {
  this->bytes = anim.get_image_bytes_size();
  this->size = 0U;
  this->frame_index = frame_index;

  auto frame = anim.get_frame(frame_index);
  for (usize i = 0U; i < frame.get_layer_count(); ++i) {
    if (frame.get_image_id(i)) {
      ++this->size;
    }
  }

  if (this->size == 0U) {
    this->data = nullptr;
    return;
  }

  this->data =
      static_cast<draw::data_ptr>(caretaker.allocate(this->calculate_size()));

  auto* cursor = this->data;
  RemoveFrameImage* img = nullptr;
  usize index = 0U;
  usize id = 0U;
  for (usize layer_index = 0U; index < this->size; ++layer_index) {
    id = frame.get_image_id(layer_index);
    if (id == 0U) {
      continue;
    }

    ++index;
    img = reinterpret_cast<RemoveFrameImage*>(cursor);
    cursor += this->bytes + sizeof(usize);
    img->layer_index = layer_index;
    std::memcpy(
        img->pixels, *TRY_ABORT_RET(anim.get_pixels(id), "Could not read anim"),
        this->bytes
    );
  }
}

usize RemoveFrame::calculate_size() const noexcept {
  return this->size * (this->bytes + sizeof(usize));
}

void RemoveFrame::undo(Model& model) const noexcept {
  // Re-add the frame
  TRY_ABORT(
      model.anim.insert_frame(this->frame_index), "Could not insert frame"
  );

  // Write the data
  auto* cursor = this->data;
  RemoveFrameImage* img = nullptr;
  usize id = 0U;
  for (usize i = 0U; i < this->size; ++i) {
    img = reinterpret_cast<RemoveFrameImage*>(cursor);
    id = *TRY_ABORT_RET(
        model.anim.create_image(this->frame_index, img->layer_index),
        "Could not create image"
    );
    model.anim.get_image_fast(id).set_pixels(img->pixels, this->bytes);

    cursor += this->bytes + sizeof(usize);
  }

  // Update model
  model.frame_index = this->frame_index;
  id = model.anim.get_image_id(model.frame_index, model.layer_index);
  if (model.img_id == id) {
    return;
  }

  model.img_id = id;
  if (id > 0U) {
    model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Could not read anim");
  }
}

void RemoveFrame::redo(Model& model) const noexcept {
  TRY_ABORT(
      model.anim.remove_frame(this->frame_index), "Could not removve frame"
  );

  if (this->frame_index <= model.frame_index) {
    --model.frame_index;
  }

  auto id = model.anim.get_image_id(model.frame_index, model.layer_index);
  if (model.img_id == id) {
    return;
  }

  model.img_id = id;
  if (id > 0U) {
    model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Could not read anim");
  }
}

#ifndef NDEBUG

void RemoveFrame::print(bool lock) const noexcept {
  // TODO: Add for debugging
}

#endif

} // namespace history

