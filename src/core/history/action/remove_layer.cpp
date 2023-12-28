/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-16
 *===============================*/

#include "./remove_layer.hpp"
#include "core/draw/types.hpp"
#include "core/history/caretaker.hpp"
#include "core/logger/logger.hpp"
#include <cassert>
#include <cstring>

namespace history {

struct RemoveLayerImage {
  usize frame_index = 0U;
  draw::data_type pixels[]; // NOLINT
};

void RemoveLayer::init(
    draw::Anim& anim, usize layer_index, Caretaker& caretaker
) noexcept {
  this->bytes = anim.get_image_bytes_size();
  this->size = 0U;
  this->layer_index = layer_index;

  for (auto it = anim.get_frame_iter(); it; ++it) {
    if (it.get_image_id(layer_index)) {
      ++this->size;
    }
  }

  this->data =
      static_cast<draw::data_ptr>(caretaker.allocate(this->calculate_size()));
  *reinterpret_cast<draw::LayerInfo*>(this->data) =
      anim.get_layer_info(this->layer_index);

  // Record the data of the remove layer
  auto* cursor = this->data + sizeof(draw::LayerInfo);
  RemoveLayerImage* img = nullptr;
  usize index = 0U;
  usize id = 0U;
  for (auto it = anim.get_frame_iter(); index < this->size; ++it) {
    id = it.get_image_id(this->layer_index);
    if (id == 0U) {
      continue;
    }

    ++index;
    img = reinterpret_cast<RemoveLayerImage*>(cursor);
    cursor += this->bytes + sizeof(usize);
    img->frame_index = it.get_index();
    std::memcpy(
        img->pixels, *TRY_ABORT_RET(anim.get_pixels(id), "Could not read anim"),
        this->bytes
    );
  }
}

usize RemoveLayer::calculate_size() const noexcept {
  return sizeof(draw::LayerInfo) + this->size * (this->bytes + sizeof(usize));
}

void RemoveLayer::undo(Model& model) const noexcept {
  // Re-add the layer
  TRY_ABORT(
      model.anim.insert_layer(this->layer_index), "Could not insert layer"
  );
  model.anim.set_layer_info(
      this->layer_index, *reinterpret_cast<draw::LayerInfo*>(this->data)
  );

  // Write the data
  auto* cursor = this->data + sizeof(draw::LayerInfo);
  RemoveLayerImage* img = nullptr;
  usize id = 0U;
  for (usize i = 0U; i < this->size; ++i) {
    img = reinterpret_cast<RemoveLayerImage*>(cursor);
    id = *TRY_ABORT_RET(
        model.anim.create_image(img->frame_index, this->layer_index),
        "Could not create image"
    );
    model.anim.get_image_fast(id).set_pixels(img->pixels, this->bytes);

    cursor += this->bytes + sizeof(usize);
  }

  // Update model
  id = model.anim.get_image_id(model.frame_index, model.layer_index);
  if (model.img_id == id) {
    return;
  }

  model.img_id = id;
  if (id > 0U) {
    model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Could not read anim");
  }
}

void RemoveLayer::redo(Model& model) const noexcept {
  TRY_ABORT(
      model.anim.remove_layer(this->layer_index), "Could not remove layer"
  );

  if (this->layer_index < model.layer_index) {
    --model.layer_index;
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

void RemoveLayer::print(bool lock) const noexcept {
  LOCK_PRINT(logger::Level::DEBUG_LVL, "Edit Image", lock);

  // Info
  logger::print(
      "  Bytes: " USIZE_FMT ";  Size: " USIZE_FMT ";  Layer: " USIZE_FMT "\n",
      this->bytes, this->size, this->layer_index
  );

  auto* cursor = this->data + sizeof(draw::LayerInfo);
  RemoveLayerImage* img = nullptr;
  for (usize i = 0U; i < this->size; ++i) {
    img = reinterpret_cast<RemoveLayerImage*>(cursor);
    cursor += this->bytes + sizeof(usize);
    logger::print("  Frame: " USIZE_FMT "\n", img->frame_index);

    for (usize j = 0U; j < this->bytes; ++j) {
      logger::print("%02x", img->pixels[j]);
    }
    logger::print("\n");
  }

  UNLOCK_PRINT(lock);
}

#endif

} // namespace history

