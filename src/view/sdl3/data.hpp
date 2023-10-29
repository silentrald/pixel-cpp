/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#ifndef PXL_VIEW_SDL3_DATA_HPP
#define PXL_VIEW_SDL3_DATA_HPP

namespace view::sdl3 {

namespace widget {

class Textbox;

} // namespace widget

/**
 * Mutable data being passed on the input functions of widgets.
 * Mostly references of selected widgets.
 **/
struct Data {
  widget::Textbox* sel_textbox = nullptr;
  widget::Textbox* new_sel_textbox = nullptr;
};

} // namespace view::sdl3

#endif

