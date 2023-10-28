/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-26
 *==========================*/

#include "./status.hpp"

namespace view::sdl3::widget {

void StatusBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  // TODO: UwU
}

void StatusBox::reset() noexcept {
  // TODO:
}

void StatusBox::input(const event::Input& evt) noexcept {
  //
}

void StatusBox::update() noexcept {
  //
}

void StatusBox::render(const Renderer& renderer) const noexcept {
  renderer.set_color({0x00, 0x88, 0x00, 0xff});
  renderer.fill_rect(this->rect);

  if (this->pos.x < 0 || this->pos.y < 0 || this->pos.x >= this->size.x ||
      this->pos.y >= this->size.y) {
    return;
  }

  auto num_rect = renderer.render_number(
      this->pos.y + 1, {this->rect.x + this->rect.w, this->rect.y}
  );
  num_rect.x -= 8.0F;
  num_rect = renderer.render_number(this->pos.x + 1, num_rect.pos);
}

} // namespace view::sdl3::widget

