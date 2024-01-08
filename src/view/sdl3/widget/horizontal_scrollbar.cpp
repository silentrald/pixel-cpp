/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-17
 *==========================*/

#include "./horizontal_scrollbar.hpp"

namespace view::sdl3::widget {

void HorizontalScrollbar::set_x(f32 x) noexcept {
  this->x = x;
  this->scroll_rect.x = x;
}

void HorizontalScrollbar::set_y(f32 y) noexcept {
  this->y = y;
  this->scroll_rect.y = y;
}

void HorizontalScrollbar::set_width(f32 width) noexcept {
  this->w = width;
  if (this->real_width > 0.0F) {
    this->adjust_widths();
  }
}

void HorizontalScrollbar::set_height(f32 height) noexcept {
  this->h = height;
  this->scroll_rect.h = height;
}

void HorizontalScrollbar::set_real_width(f32 real_width) noexcept {
  this->real_width = real_width;
  if (real_width < this->w) {
    this->scroll_rect.w = this->w;
    return;
  }

  this->adjust_widths();
}

void HorizontalScrollbar::adjust_widths() noexcept {
  this->scroll_rect.w = this->w / real_width * this->w;
  this->max_offset = this->x + this->w - this->scroll_rect.w;

  // Adjust offset
  this->scroll_rect.x = std::min(this->scroll_rect.x, this->max_offset);
}

void HorizontalScrollbar::set_percentage(f32 percentage) noexcept {
  this->scroll_rect.x = percentage * (this->max_offset - this->x) + this->x;
}

f32 HorizontalScrollbar::get_percentage() const noexcept {
  return (this->scroll_rect.x - this->x) / (this->max_offset - this->x);
}

void HorizontalScrollbar::reset() noexcept {
  // Do nothing UwU
}

void HorizontalScrollbar::update_locale() noexcept {
  // Do nothing UwU
}

void HorizontalScrollbar::input(const event::Input& evt) noexcept {
  if (evt.mouse.left == input::MouseState::DOWN &&
      this->scroll_rect.has_point(evt.mouse.pos)) {
    data::set_f1(this->scroll_rect.x);
    data::set_f2(evt.mouse.pos.x);
    data::set_left_click_widget(this);
    return;
  }

  if (data::get_left_click_widget() != this ||
      evt.mouse.left != input::MouseState::HOLD) {
    return;
  }

  this->scroll_rect.x = std::clamp(
      data::get_f1() - data::get_f2() + evt.mouse.pos.x, this->x,
      this->max_offset
  );
}

void HorizontalScrollbar::update(f32 delta) noexcept {
  // Do nothing UwU
}

void HorizontalScrollbar::render() noexcept {
  renderer::set_color({0x88, 0x88, 0x88, 0xff});
  renderer::fill_rect(this->rect);

  renderer::set_color({0xff, 0xff, 0xff, 0xff});
  renderer::fill_rect(this->scroll_rect);
}

} // namespace view::sdl3::widget
