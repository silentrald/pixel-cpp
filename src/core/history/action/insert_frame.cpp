/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#include "./insert_frame.hpp"

namespace history {

void InsertFrame::undo(Model& model) const noexcept {
  TRY_ABORT(model.anim.remove_frame(this->frame_index), "Anim remove frame");
  model.frame_index = this->prev_frame_index;
  model.img_id = model.anim.get_image_id(model.frame_index, model.layer_index);
  if (model.img_id > 0U) {
    model.img = *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
  }
}

void InsertFrame::redo(Model& model) const noexcept {
  model.frame_index = this->frame_index;
  TRY_ABORT(model.anim.insert_frame(this->frame_index), "Anim insert frame")
  model.img_id = 0U;
}

} // namespace history

