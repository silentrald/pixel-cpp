/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-19
 *===============================*/

#ifndef PXL_HISTORY_ARENA_HPP
#define PXL_HISTORY_ARENA_HPP

#include "types.hpp"

namespace history {

class Arena {
public:
  Arena() noexcept;
  Arena(const Arena&) noexcept = delete;
  Arena& operator=(const Arena&) noexcept = delete;
  Arena(Arena&& rhs) noexcept;
  Arena& operator=(Arena&& rhs) noexcept;
  ~Arena() noexcept;

  [[nodiscard]] error_code init(usize alloc_size) noexcept;

  void set_cursor_with_ptr(void* new_data) noexcept;
  [[nodiscard]] void* allocate(usize size) noexcept;

  [[nodiscard]] void* get_data() const noexcept;
  [[nodiscard]] void* get_cursor_data() const noexcept;

  [[nodiscard]] usize get_available() const noexcept;

  void reset() noexcept;

private:
  // For maximum allocation for undo/redo stack
  u8* data;
  usize cursor;
  usize alloc_size;
};

} // namespace history

#endif

