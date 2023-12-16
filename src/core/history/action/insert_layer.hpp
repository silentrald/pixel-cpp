/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#ifndef HISTORY_ACTION_INSERT_LAYER_HPP
#define HISTORY_ACTION_INSERT_LAYER_HPP

#include "./types.hpp"
#include "model/model.hpp"

namespace history {

struct InsertLayer {
  usize prev_layer_index = 0U; // prev layer_index
  usize layer_index = 0U;      // created layer_index

  void undo(Model& model) const noexcept;
  void redo(Model& model) const noexcept;
};

} // namespace history

#endif

