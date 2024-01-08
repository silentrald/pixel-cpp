/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-17
 *==========================*/

#include "./vertical_scrollbar.hpp"

namespace view::sdl3::widget {

void VerticalScrollbar::set_x(f32 x) noexcept {
  this->x = x;
  this->scroll_rect.x = x;
}

void VerticalScrollbar::set_y(f32 y) noexcept {
  this->y = y;
  this->scroll_rect.y = y;
}

void VerticalScrollbar::set_width(f32 width) noexcept {
  this->w = width;
  this->scroll_rect.w = width;
}

void VerticalScrollbar::set_height(f32 height) noexcept {
  this->h = height;
  if (this->real_height > 0.0F) {
    this->adjust_heights();
  }
}

void VerticalScrollbar::set_real_height(f32 real_height) noexcept {
  this->real_height = real_height;
  if (real_height < this->h) {
    this->scroll_rect.h = this->h;
    return;
  }

  this->adjust_heights();
}

void VerticalScrollbar::adjust_heights() noexcept {
  this->scroll_rect.h = this->h / real_height * this->h;
  this->max_offset = this->y + this->h - this->scroll_rect.h;

  // Adjust offset
  this->scroll_rect.y = std::min(this->scroll_rect.y, this->max_offset);
}

void VerticalScrollbar::set_percentage(f32 percentage) noexcept {
  this->scroll_rect.y = percentage * (this->max_offset - this->y) + this->y;
}

f32 VerticalScrollbar::get_percentage() const noexcept {
  return (this->scroll_rect.y - this->y) / (this->max_offset - this->y);
}

void VerticalScrollbar::reset() noexcept {
  // Do nothing UwU
}

void VerticalScrollbar::update_locale() noexcept {
  // Do nothing UwU
}

void VerticalScrollbar::input(const event::Input& evt) noexcept {
  if (evt.mouse.left == input::MouseState::DOWN &&
      this->scroll_rect.has_point(evt.mouse.pos)) {
    data::set_f1(this->scroll_rect.y);
    data::set_f2(evt.mouse.pos.y);
    data::set_left_click_widget(this);
    return;
  }

  if (data::get_left_click_widget() != this ||
      evt.mouse.left != input::MouseState::HOLD) {
    return;
  }

  this->scroll_rect.y = std::clamp(
      data::get_f1() - data::get_f2() + evt.mouse.pos.y, this->y,
      this->max_offset
  );
}

void VerticalScrollbar::update(f32 delta) noexcept {
  // Do nothing UwU
}

void VerticalScrollbar::render() noexcept {
  renderer::set_color({0x88, 0x88, 0x88, 0xff});
  renderer::fill_rect(this->rect);

  renderer::set_color({0xff, 0xff, 0xff, 0xff});
  renderer::fill_rect(this->scroll_rect);
}

} // namespace view::sdl3::widget
