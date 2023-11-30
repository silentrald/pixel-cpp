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

struct KeyPress {
  ds::stack_vector<c8, 8> keys{};
};

} // namespace event

#endif

