/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#ifndef MODULES_HISTORY_CARETAKER_HPP
#define MODULES_HISTORY_CARETAKER_HPP

#include "./action.hpp"
#include "./arena.hpp"
#include "core/draw/types.hpp"
#include "core/ds/vector.hpp"
#include "model/model.hpp"

namespace history {

// NOTE: Add memory arena here, to better store the animation data
class Caretaker {
public:
  Caretaker(const Caretaker&) noexcept = delete;
  Caretaker& operator=(const Caretaker&) noexcept = delete;

  Caretaker() noexcept;
  Caretaker(Caretaker&&) noexcept = default;
  Caretaker& operator=(Caretaker&&) noexcept = default;
  ~Caretaker() noexcept = default;

  /**
   * First call to initialize or re-initialize the caretaker
   **/
  [[nodiscard]] error_code init(i32 capacity) noexcept;

  /**
   * Call before push_action, to the arena memory and cursors
   **/
  void prepare_push_action() noexcept;

  /**
   * Pushes a new snapshot to the caretaker.
   * Call prepare_push_action first before calling this function
   **/
  void push_action(Action&& action) noexcept;

  [[nodiscard]] Action create_edit_image_action(
      draw::data_ptr prev_pixels, draw::data_ptr pixels, usize frame_index,
      usize layer_index, usize bytes
  ) noexcept;
  [[nodiscard]] void* allocate(usize bytes) noexcept;

  [[nodiscard]] bool can_undo() const noexcept;
  [[nodiscard]] bool undo(Model& model) noexcept;
  [[nodiscard]] bool can_redo() const noexcept;
  [[nodiscard]] bool redo(Model& model) noexcept;

private:
  // NOTE: Can be optimized with using a better datastructure with less overhead
  ds::vector<Action> actions{};
  i32 cursor;
  i32 start_cursor;
  i32 end_cursor;

  Arena arena{};

  void move_start_cursor(void* orig_ptr) noexcept;
  void invalidate_actions(void* start_ptr, void* end_ptr) noexcept;
  void invalidate_actions_end(void* start_ptr) noexcept;
  void invalidate_actions_start(void* end_ptr) noexcept;

  void move_arena_cursor() noexcept;
};

} // namespace history

#endif

