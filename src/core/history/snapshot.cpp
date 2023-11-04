/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#include "./snapshot.hpp"

namespace history {

void Snapshot::snap(const Model& model) noexcept {
  this->anim.minicopy(model.anim);
  this->frame_id = model.frame_id;
  this->layer_index = model.layer_index;
}

void Snapshot::restore(Model& model) const noexcept {
  model.anim.copy(this->anim);
  model.frame_id = this->frame_id;
  model.layer_index = this->layer_index;
}

void Snapshot::reset() noexcept {
  this->anim.clear();
  this->frame_id = 0U;
  this->layer_index = -1;
}

} // namespace history
