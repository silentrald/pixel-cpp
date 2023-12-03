/*===============================*
 * author/s:
 *  - silentrald
 * version: 1.0
 * created: 2023-10-18
 *===============================*/

#ifndef PXL_EVENT_HPP
#define PXL_EVENT_HPP

#include "core/ds/stack_vector.hpp"
#include "types.hpp"
#include "view/input.hpp"

namespace event {

enum Flag : u32 {
  NONE = 0x0000'0000,

  SNAPSHOT = 0x0000'0001,
};

struct Input {
  struct {
    // Position on where the mouse is
    fvec pos{};

    // Left mouse button state
    input::MouseState left{};

    // Right mouse button state
    input::MouseState right{};

    // Middle mouse button state
    input::MouseState middle{};

    // Scroll Info
    fvec wheel{};
  } mouse;

  struct {
    input::KeyMod mods{};
  } key;

  [[nodiscard]] inline bool is_mouse_state(input::MouseState state
  ) const noexcept {
    return this->mouse.left == state || this->mouse.right == state ||
           this->mouse.middle == state;
  }
};

class KeyPress {
public:
  void push_char(c8 c) noexcept {
    this->keys.push_back(c);
  }

  [[nodiscard]] c8 get_next_char() noexcept {
    assert(this->has_next());
    return this->keys[this->cursor++];
  }

  [[nodiscard]] c8 get_prev_char() const noexcept {
    assert(this->cursor > 0U);
    return this->keys[this->cursor - 1];
  }

  void to_next_section() noexcept {
    for (; this->cursor < this->keys.get_size(); ++this->cursor) {
      if (this->keys[this->cursor] == input::Keycode::TAB) {
        ++this->cursor;
        return;
      }
    }
  }

  [[nodiscard]] bool has_next() const noexcept {
    return this->cursor < this->keys.get_size();
  }

  [[nodiscard]] bool has_keys() const noexcept {
    return !this->keys.is_empty();
  }

  void reset() noexcept {
    this->keys.clear();
    this->cursor = 0U;
  }

private:
  ds::stack_vector<c8, 8> keys{};
  u32 cursor = 0U;
};

} // namespace event

#endif

