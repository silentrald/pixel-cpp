/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#include "./pan.hpp"
#include <algorithm>

namespace tool {

u32 Pan::execute(Model& model, const event::Input& evt) noexcept {
  switch (evt.mouse.middle) {
  case input::MouseState::HOLD:
  case input::MouseState::UP:
    this->handle_mouse_motion(model, evt.mouse.pos);
    break;

  case input::MouseState::DOWN:
    this->handle_mouse_down(model, evt.mouse.pos);
    break;

  default:
    // Do nothing UwU
    break;
  }

  return event::Flag::NONE;
}

void Pan::handle_mouse_down(Model& model, fvec pos) noexcept {
  this->orig_pos = model.rect.pos;
  this->origin = pos;
}

void Pan::handle_mouse_motion(Model& model, fvec pos) const noexcept {
  model.rect.pos = {
      this->orig_pos.x + pos.x - this->origin.x,
      this->orig_pos.y + pos.y - this->origin.y};

  // Clamping
  model.rect.x = std::clamp(
      model.rect.x, model.bounds.x - model.rect.w + 32.0F,
      model.bounds.x + model.bounds.w - 32.0F
  );
  model.rect.y = std::clamp(
      model.rect.y, model.bounds.y - model.rect.h + 32.0F,
      model.bounds.y + model.bounds.h - 32.0F
  );
}

} // namespace tool
