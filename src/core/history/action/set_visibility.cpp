/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#include "./set_visibility.hpp"

namespace history {

void SetVisibility::undo(Model& model) const noexcept {
  model.anim.set_layer_visibility(this->layer_index, !this->visibility);
}

void SetVisibility::redo(Model& model) const noexcept {
  model.anim.set_layer_visibility(this->layer_index, this->visibility);
}

} // namespace history

