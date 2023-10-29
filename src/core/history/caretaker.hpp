/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#ifndef MODULES_HISTORY_CARETAKER_HPP
#define MODULES_HISTORY_CARETAKER_HPP

#include "./snapshot.hpp"
#include "core/ds/vector.hpp"

namespace history {

class Caretaker {
public:
  Caretaker(const Caretaker&) noexcept = delete;
  Caretaker& operator=(const Caretaker&) noexcept = delete;

  explicit Caretaker(i32 capacity = 50) noexcept;
  Caretaker(Caretaker&&) noexcept = default;
  Caretaker& operator=(Caretaker&&) noexcept = default;
  ~Caretaker() noexcept = default;

  /**
   * First call to initialize or re-initialize the caretaker
   **/
  void init(Snapshot&& snapshot) noexcept;

  /**
   * Pushes a new snapshot to the caretaker
   * call init first before calling this function
   **/
  void push_snapshot(Snapshot&& snapshot) noexcept;

  [[nodiscard]] bool can_undo() const noexcept;
  [[nodiscard]] const Snapshot& undo() noexcept;
  [[nodiscard]] bool can_redo() const noexcept;
  [[nodiscard]] const Snapshot& redo() noexcept;

private:
  // NOTE: Can be optimized with using a better datastructure with less overhead
  // Only needs a container that does not grow but can be resized
  ds::vector<Snapshot> snapshots{};
  i32 cursor = -1;
  i32 start_cursor = -1;
  i32 end_cursor = -1;

  void clear() noexcept;

  /**
   * When pushing a new snapshot in a middle timeline,
   * future snapshots needs to be freed
   **/
  void remove_future_snapshots() noexcept;

  /**
   * Updates start_cursor and end_cursor
   **/
  void update_cursors() noexcept;
};

} // namespace history

#endif

