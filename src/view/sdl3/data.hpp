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

class Widget;

namespace widget {

class Input;

} // namespace widget

namespace data {

// === Pointers === //

[[nodiscard]] widget::Input* get_selected_input() noexcept;
void set_selected_input(widget::Input* input) noexcept;

[[nodiscard]] widget::Input* get_new_selected_input() noexcept;
void set_new_selected_input(widget::Input* input) noexcept;

[[nodiscard]] widget::Input* get_first_input() noexcept;
void set_first_input(widget::Input* input) noexcept;

[[nodiscard]] Widget* get_left_click_widget() noexcept;
void set_left_click_widget(Widget* widget) noexcept;

[[nodiscard]] Widget* get_right_click_widget() noexcept;
void set_right_click_widget(Widget* widget) noexcept;

[[nodiscard]] Widget* get_middle_click_widget() noexcept;
void set_middle_click_widget(Widget* widget) noexcept;

// === Temp Values === //

[[nodiscard]] f32 get_f1() noexcept;
[[nodiscard]] f32 get_f2() noexcept;
[[nodiscard]] f32 get_f3() noexcept;
[[nodiscard]] f32 get_f4() noexcept;
[[nodiscard]] fvec get_fvec1() noexcept;
[[nodiscard]] fvec get_fvec2() noexcept;

void set_f1(f32 f) noexcept;
void set_f2(f32 f) noexcept;
void set_f3(f32 f) noexcept;
void set_f4(f32 f) noexcept;
void set_fvec1(fvec vec) noexcept;
void set_fvec2(fvec vec) noexcept;

// === Events === //

[[nodiscard]] bool is_dragging() noexcept;
void set_dragging(bool val) noexcept;

// TODO: Check if this needs to be reset
[[nodiscard]] bool is_clear_selected() noexcept;
void set_clear_selected(bool val) noexcept;

// Resets to false when called
[[nodiscard]] bool is_left_click() noexcept;
void set_left_click(bool click) noexcept;

// Resets to false when called
[[nodiscard]] bool is_right_click() noexcept;
void set_right_click(bool click) noexcept;

// Resets to false when called
[[nodiscard]] bool is_middle_click() noexcept;
void set_middle_click(bool click) noexcept;

} // namespace data

} // namespace view::sdl3

#endif

