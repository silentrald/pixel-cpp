/*===============================*
 * author/s:
 *  - silentrald
 * version: 1.0
 * created: 2023-10-18
 *===============================*/

#ifndef PXL_EVENT_HPP
#define PXL_EVENT_HPP

#include "types.hpp"
#include "view/input.hpp"
#include <array>

namespace event {

enum Flag : u32 {
  NONE = 0x0000'0000,

  SNAPSHOT = 0x0000'0001,
};

struct Input {
  struct {
    // Position on where the mouse is
    fvec pos{};

    // Left mouse button info
    struct {
      input::MouseState state{};
    } left;

    // Right mouse button info
    struct {
      input::MouseState state{};
    } right;

    // Middle mouse button info
    struct {
      input::MouseState state{};
    } middle;

    // Scroll Info
    fvec wheel{};
  } mouse;

  struct {
    std::array<c8, 8> pressed{}; // limit to only 8 chars
    input::KeyMod mods{};
  } key;

  [[nodiscard]] inline bool is_mouse_state(input::MouseState state
  ) const noexcept {
    return this->mouse.left.state == state ||
           this->mouse.right.state == state ||
           this->mouse.middle.state == state;
  }
};

} // namespace event

#endif

