/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#include "./snapshot.hpp"
#include "core/logger/logger.hpp"

namespace history {

void Snapshot::snap_default(const Model& model) noexcept {
  this->frame_id = model.frame_id;
  this->layer_index = model.layer_index;
}

void Snapshot::snap_anim(const Model& model) noexcept {
  this->anim.minicopy(model.anim);
}

void Snapshot::restore(Model& model) const noexcept {
  model.anim.copy(this->anim);

  // Update the disk backup file
  model.anim.write_pixels_to_disk(model.img_id);

  model.img_id = model.anim.get_image_id(this->frame_id, this->layer_index);

  // Update the image object, since this can be invalidated in anim copy
  model.img = model.anim.get_image(model.img_id);

  model.frame_id = this->frame_id;
  model.layer_index = this->layer_index;
}

void Snapshot::reset() noexcept {
  this->anim.clear();
  this->frame_id = 0U;
  this->layer_index = -1;
}

} // namespace history
