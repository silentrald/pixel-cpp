/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#ifndef HISTORY_ACTION_EDIT_IMAGE_HPP
#define HISTORY_ACTION_EDIT_IMAGE_HPP

#include "core/draw/types.hpp"
#include "model/model.hpp"

namespace history {

struct EditImage {
  // Pointers can be disjointed
  draw::data_ptr prev_pixels = nullptr;
  draw::data_ptr pixels = nullptr;
  usize frame_index = 0U;
  usize layer_index = 0U;
  bool new_image = false;

  void undo(Model& model) const noexcept;
  void redo(Model& model) const noexcept;

#ifndef NDEBUG
  void print(bool lock = true) const noexcept;
#endif
};

} // namespace history

#endif

