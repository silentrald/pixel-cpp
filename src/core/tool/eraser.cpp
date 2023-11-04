/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#include "./eraser.hpp"
#include "./utils.hpp"
#include "view/sdl3/texture.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <type_traits>
#include <utility>

namespace tool {

u32 Eraser::execute(Model& model, const event::Input& evt) noexcept {
  switch (evt.mouse.left.state) {
  case input::MouseState::HOLD:
    this->handle_mouse_motion(model, evt.mouse.pos);
    return event::Flag::NONE;

  case input::MouseState::UP:
    this->handle_mouse_motion(model, evt.mouse.pos);
    return event::Flag::SNAPSHOT;

  case input::MouseState::DOWN:
    this->handle_mouse_down(model, evt.mouse.pos);
    return event::Flag::NONE;

  default:
    return event::Flag::NONE;
  }
}

/**
 * Uses:
 *   model.tex1 - current image
 **/
void Eraser::handle_mouse_down(Model& model, fvec pos) noexcept {
  if (!model.anim.has_point(model.curr_pos) ||
      !model.select_mask[model.get_pixel_index()]) {
    return;
  }

  model.img.paint(model.curr_pos, color::TRANSPARENT_COLOR);
  model.tex1->paint(model.get_pixel_index(), color::TRANSPARENT_COLOR);
}

/**
 * Uses:
 *   model.tex1 - current image
 **/
void Eraser::handle_mouse_motion(Model& model, fvec pos) noexcept {
  i32 distance = model.prev_pos.chebyshev_distance(model.curr_pos);
  if (distance == 0) {
    return;
  }

  // If distance is 1, just draw the point
  if (distance == 1) {
    // Check if point is in bounds
    if (!model.anim.has_point(model.curr_pos) ||
        !model.select_mask[model.get_pixel_index()]) {
      return;
    }

    model.img.paint(model.get_pixel_index(), color::TRANSPARENT_COLOR);
    model.tex1->paint(model.get_pixel_index(), color::TRANSPARENT_COLOR);
    return;
  }

  // Draw a line to the previous and current position
  utils::draw_line(
      &model.img, *model.tex1, model.anim.get_size(), model.curr_pos,
      model.prev_pos, color::TRANSPARENT_COLOR, model.select_mask
  );
}

} // namespace tool

