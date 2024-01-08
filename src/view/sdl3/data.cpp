/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-29
 *==========================*/

#include "./data.hpp"
#include "./input/input.hpp"

namespace view::sdl3 {

widget::Input* selected_input = nullptr;
widget::Input* new_selected_input = nullptr;
widget::Input* first_input = nullptr;

Widget* left_click_widget = nullptr;
Widget* right_click_widget = nullptr;
Widget* middle_click_widget = nullptr;

// Holds temp values
frect tmp_rect{};

bool dragging = false;
bool clear_selected = false;

bool left_click = false;
bool right_click = false;
bool middle_click = false;

// === Helpers === //

[[nodiscard]] inline bool get_and_reset(bool& b) {
  if (!b) {
    return false;
  }

  b = false;
  return true;
}

// === Pointers === //

widget::Input* data::get_selected_input() noexcept {
  return selected_input;
}

void data::set_selected_input(widget::Input* input) noexcept {
  selected_input = input;
}

widget::Input* data::get_new_selected_input() noexcept {
  return new_selected_input;
}

void data::set_new_selected_input(widget::Input* input) noexcept {
  new_selected_input = input;
}

widget::Input* data::get_first_input() noexcept {
  return first_input;
}

void data::set_first_input(widget::Input* input) noexcept {
  first_input = input;
}

Widget* data::get_left_click_widget() noexcept {
  return left_click_widget;
}

void data::set_left_click_widget(Widget* widget) noexcept {
  left_click_widget = widget;
}

Widget* data::get_right_click_widget() noexcept {
  return right_click_widget;
}

void data::set_right_click_widget(Widget* widget) noexcept {
  right_click_widget = widget;
}

Widget* data::get_middle_click_widget() noexcept {
  return middle_click_widget;
}

void data::set_middle_click_widget(Widget* widget) noexcept {
  middle_click_widget = widget;
}

// === Temp Memory Storage === //

f32 data::get_f1() noexcept {
  return tmp_rect.x;
}

f32 data::get_f2() noexcept {
  return tmp_rect.y;
}

f32 data::get_f3() noexcept {
  return tmp_rect.w;
}

f32 data::get_f4() noexcept {
  return tmp_rect.h;
}

fvec data::get_fvec1() noexcept {
  return tmp_rect.pos;
}

fvec data::get_fvec2() noexcept {
  return tmp_rect.size;
}

void data::set_f1(f32 f) noexcept {
  tmp_rect.x = f;
}

void data::set_f2(f32 f) noexcept {
  tmp_rect.y = f;
}

void data::set_f3(f32 f) noexcept {
  tmp_rect.w = f;
}

void data::set_f4(f32 f) noexcept {
  tmp_rect.h = f;
}

void data::set_fvec1(fvec vec) noexcept {
  tmp_rect.pos = vec;
}

void data::set_fvec2(fvec vec) noexcept {
  tmp_rect.size = vec;
}

// === Events === //

bool data::is_dragging() noexcept {
  return dragging;
}

void data::set_dragging(bool val) noexcept {
  dragging = val;
}

bool data::is_clear_selected() noexcept {
  return clear_selected;
}

void data::set_clear_selected(bool val) noexcept {
  clear_selected = val;
}

bool data::is_left_click() noexcept {
  return get_and_reset(left_click);
}

void data::set_left_click(bool click) noexcept {
  left_click = click;
}

bool data::is_right_click() noexcept {
  return get_and_reset(right_click);
}

void data::set_right_click(bool click) noexcept {
  right_click = click;
}

bool data::is_middle_click() noexcept {
  return get_and_reset(middle_click);
}

void data::set_middle_click(bool click) noexcept {
  middle_click = click;
}

} // namespace view::sdl3
