/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#ifndef PXL_VIEW_SDL3_DATA_HPP
#define PXL_VIEW_SDL3_DATA_HPP

#include "types.hpp"

namespace view::sdl3 {

namespace widget {

class Input;

} // namespace widget

/**
 * Mutable data being passed on the input functions of widgets.
 * Mostly references of selected widgets.
 **/
struct InputData {
  widget::Input* selected_input = nullptr;
  widget::Input* new_selected_input = nullptr;
  widget::Input* first_input = nullptr;

  fvec orig_pos{};
  fvec orig_mouse{};

  bool dragging = false;
  bool clear_selected = false;

  // For events
  bool left_click = false;
  bool right_click = false;
};

} // namespace view::sdl3

#endif

