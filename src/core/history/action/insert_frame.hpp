/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#ifndef HISTORY_ACTION_INSERT_FRAME_HPP
#define HISTORY_ACTION_INSERT_FRAME_HPP

#include "model/model.hpp"
#include "types.hpp"

namespace history {

struct InsertFrame {
  usize prev_frame_index = 0U;
  usize frame_index = 0U;

  void undo(Model& model) const noexcept;
  void redo(Model& model) const noexcept;
};

} // namespace history

#endif

