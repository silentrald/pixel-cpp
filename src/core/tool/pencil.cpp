/*===============================*
 * author/s:
 *  - silentrald
 * version: 1.0
 * created: 2023-07-27
 *===============================*/

#include "./pencil.hpp"
#include "./utils.hpp"
#include "types.hpp"

namespace tool {

u32 Pencil::execute(Model& model, const event::Input& evt) noexcept {
  // TODO: Check if there is a bug in priority of mouse type

  switch (evt.mouse.left.state) {
  case input::MouseState::HOLD:
    this->handle_mouse_motion(model);
    return event::Flag::NONE;

  case input::MouseState::UP:
    this->handle_mouse_motion(model);
    return event::Flag::SNAPSHOT;

  case input::MouseState::DOWN:
    this->handle_mouse_down(model, input::MouseType::LEFT);
    return event::Flag::NONE;

  default:
    // Do nothing
    break;
  }

  switch (evt.mouse.right.state) {
  case input::MouseState::HOLD:
    this->handle_mouse_motion(model);
    return event::Flag::NONE;

  case input::MouseState::UP:
    this->handle_mouse_motion(model);
    return event::Flag::SNAPSHOT;

  case input::MouseState::DOWN:
    this->handle_mouse_down(model, input::MouseType::RIGHT);
    return event::Flag::NONE;

  default:
    // Do nothing
    break;
  }

  return event::Flag::NONE;
}

void Pencil::handle_mouse_down(Model& model, input::MouseType type) noexcept {
  model.color =
      type == input::MouseType::LEFT ? model.fg_color : model.bg_color;

  // Don't draw the point if it is outside
  if (!model.anim.has_point(model.curr_pos) ||
      !model.select_mask[model.get_pixel_index()]) {
    return;
  }

  model.img.paint(model.get_pixel_index(), model.color);
  model.tex1->paint(model.get_pixel_index(), model.color);
}

void Pencil::handle_mouse_motion(Model& model) noexcept {
  i32 distance = model.prev_pos.chebyshev_distance(model.curr_pos);
  if (distance == 0) {
    return;
  }

  // If distance is 1, just draw the point
  if (distance == 1) {
    if (!model.anim.has_point(model.curr_pos) ||
        !model.select_mask[model.get_pixel_index()]) {
      return;
    }

    model.img.paint(model.get_pixel_index(), model.color);
    model.tex1->paint(model.get_pixel_index(), model.color);
    return;
  }

  // Draw a line to the previous and current position
  utils::draw_line(
      &model.img, *model.tex1, model.anim.get_size(), model.curr_pos,
      model.prev_pos, model.color, model.select_mask
  );
}

} // namespace tool

