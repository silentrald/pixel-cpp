/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-16
 *===============================*/

#ifndef HISTORY_ACTION_REMOVE_LAYER_HPP
#define HISTORY_ACTION_REMOVE_LAYER_HPP

#include "core/draw/anim.hpp"
#include "core/draw/types.hpp"
#include "model/model.hpp"
#include "types.hpp"

namespace history {

class Caretaker;

struct RemoveLayer {
  draw::data_ptr data = nullptr;
  usize bytes = 0U;
  usize size = 0U;
  usize layer_index = 0U;

  void init(draw::Anim& anim, usize layer_index, Caretaker& caretaker) noexcept;
  [[nodiscard]] usize calculate_size() const noexcept;

  void undo(Model& model) const noexcept;
  void redo(Model& model) const noexcept;

#ifndef NDEBUG
  void print(bool lock = true) const noexcept;
#endif
};

} // namespace history

#endif

