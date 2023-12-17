/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-17
 *===============================*/

#ifndef HISTORY_ACTION_REMOVE_FRAME_HPP
#define HISTORY_ACTION_REMOVE_FRAME_HPP

#include "core/draw/anim.hpp"
#include "core/draw/types.hpp"
#include "model/model.hpp"
#include "types.hpp"

namespace history {

class Caretaker;

struct RemoveFrame {
  draw::data_ptr data = nullptr;
  usize bytes = 0U;
  usize size = 0U;
  usize frame_index = 0U;

  void init(draw::Anim& anim, usize frame_index, Caretaker& caretaker) noexcept;

  void undo(Model& model) const noexcept;
  void redo(Model& model) const noexcept;

#ifndef NDEBUG
  void print(bool lock = true) const noexcept;
#endif

private:
  [[nodiscard]] usize calculate_size() const noexcept;
};

} // namespace history

#endif

