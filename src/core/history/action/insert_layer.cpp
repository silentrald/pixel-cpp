/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#include "./insert_layer.hpp"

namespace history {

void InsertLayer::undo(Model& model) const noexcept {
  TRY_ABORT(model.anim.remove_layer(this->layer_index), "Anim remove layer");
  model.layer_index = this->prev_layer_index;
  model.img_id = model.anim.get_image_id(model.frame_index, model.layer_index);
  if (model.img_id > 0U) {
    model.img = *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
  }
}

void InsertLayer::redo(Model& model) const noexcept {
  model.layer_index = this->layer_index;
  TRY_ABORT(model.anim.insert_layer(this->layer_index), "Anim insert layer");
  model.img_id = 0U;
}

} // namespace history

