/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#include "./snapshot.hpp"

namespace history {

void Snapshot::snap(const Model& model) noexcept {
  this->anim.copy(model.anim);
  this->frame_index = model.frame_index;
  this->layer_index = model.layer_index;
}

void Snapshot::restore(Model& model) const noexcept {
  model.anim.copy(this->anim);
  model.frame_index = this->frame_index;
  model.layer_index = this->layer_index;
}

void Snapshot::reset() noexcept {
  this->anim.clear();
  this->frame_index = this->layer_index = -1;
}

} // namespace history
