/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#ifndef HISTORY_ACTION_SET_VISIBILITY_HPP
#define HISTORY_ACTION_SET_VISIBILITY_HPP

#include "model/model.hpp"
#include "types.hpp"

namespace history {

struct SetVisibility {
  usize layer_index = 0U;
  bool visibility = false;

  void undo(Model& model) const noexcept;
  void redo(Model& model) const noexcept;
};

} // namespace history

#endif

