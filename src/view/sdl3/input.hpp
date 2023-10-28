/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-08
 *==========================*/

#ifndef PXL_VIEW_SDL3_INPUT_HPP
#define PXL_VIEW_SDL3_INPUT_HPP

namespace view::sdl3 {

// Forward declare
namespace widget {

class Textbox;

} // namespace widget

/**
 * Mutable data being passed on the input functions of widgets
 **/
struct InputData {
  widget::Textbox* selected_textbox = nullptr;
};

} // namespace view::sdl3

#endif

