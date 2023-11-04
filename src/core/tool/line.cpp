/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#include "./line.hpp"
#include "./utils.hpp"
#include "math.hpp"
#include <cmath>
#include <limits>

namespace tool {

// NOLINTNEXTLINE
const f32 RADIANS[]{
    0.000000F, // atan(0)
    0.321750F, // atan(1/3)
    0.463647F, // atan(1/2)
    0.785398F, // atan(1)
    1.107148F, // atan(2)
    1.249045F, // atan(3)
    1.570796F  // atan(inf)
};

u32 Line::execute(Model& model, const event::Input& evt) noexcept {
  // TODO: Check if there is a bug in priority of mouse type

  switch (evt.mouse.left.state) {
  case input::MouseState::HOLD:
    this->handle_mouse_motion(model, evt.key.mods, evt.mouse.pos);
    return event::Flag::NONE;

  case input::MouseState::DOWN:
    this->handle_mouse_down(model, input::MouseType::LEFT, evt.mouse.pos);
    return event::Flag::NONE;

  case input::MouseState::UP:
    this->handle_mouse_up(model, evt.key.mods, evt.mouse.pos);
    return event::Flag::SNAPSHOT;

  default:
    // Do nothing UwU
    break;
  }

  switch (evt.mouse.right.state) {
  case input::MouseState::HOLD:
    this->handle_mouse_motion(model, evt.key.mods, evt.mouse.pos);
    return event::Flag::NONE;

  case input::MouseState::DOWN:
    this->handle_mouse_down(model, input::MouseType::RIGHT, evt.mouse.pos);
    return event::Flag::NONE;

  case input::MouseState::UP:
    this->handle_mouse_up(model, evt.key.mods, evt.mouse.pos);
    return event::Flag::SNAPSHOT;

  default:
    // Do nothing UwU
    break;
  }

  return event::Flag::NONE;
}

void Line::handle_mouse_down(
    Model& model, input::MouseType type, fvec pos
) noexcept {
  // TODO: Can be used with origin
  this->origin = model.curr_pos;
  this->center.x = model.rect.x + model.scale * (this->origin.x + 0.5F);
  this->center.y = model.rect.y + model.scale * (this->origin.y + 0.5F);
  model.color =
      type == input::MouseType::LEFT ? model.fg_color : model.bg_color;

  if (!model.anim.has_point(model.curr_pos) ||
      !model.select_mask[model.get_pixel_index()]) {
    return;
  }

  model.tex2->paint(model.get_pixel_index(), model.color);
}

ivec Line::get_snapping_point(fvec pos, fvec offset, i32 scale) const noexcept {
  pos.x -= this->center.x;
  pos.y -= this->center.y;

  f32 rad = std::fmod(std::atan(std::fabs(pos.y / pos.x)), math::PI / 2.0F);
  i32 i = 1;
  for (; i < 6 && rad > RADIANS[i]; ++i) {
  }

  rad = RADIANS[i - (RADIANS[i] - rad > rad - RADIANS[i - 1])];
  f32 h = std::sqrt(pos.x * pos.x + pos.y * pos.y);
  pos.x = this->center.x + (pos.x > 0.0F ? 1.0F : -1.0F) * std::cos(rad) * h;
  pos.y = this->center.y + (pos.y > 0.0F ? 1.0F : -1.0F) * std::sin(rad) * h;
  return {
      .x = (i32)std::floor((pos.x - offset.x) / scale),
      .y = (i32)std::floor((pos.y - offset.y) / scale),
  };
}

/**
 * Uses:
 *   model.tex2 - blank image
 **/
void Line::handle_mouse_motion(
    Model& model, input::KeyMod key, fvec pos
) noexcept {
  if (model.curr_pos == model.prev_pos) {
    return;
  }

  // Delete the previous line
  utils::draw_line(
      nullptr, *model.tex2, model.anim.get_size(), this->origin, model.prev_pos,
      color::TRANSPARENT_COLOR, model.select_mask
  );

  if (key.shift) {
    model.curr_pos = this->get_snapping_point(pos, model.rect.pos, model.scale);
  }

  // Draw the new line
  utils::draw_line(
      nullptr, *model.tex2, model.anim.get_size(), this->origin, model.curr_pos,
      model.color, model.select_mask
  );
}

/**
 * Uses:
 *   model.tex1 - current image
 *   model.tex2 - empty image
 **/
void Line::handle_mouse_up(Model& model, input::KeyMod key, fvec pos) noexcept {
  // Delete the previous line
  utils::draw_line(
      nullptr, *model.tex2, model.anim.get_size(), this->origin, model.prev_pos,
      color::TRANSPARENT_COLOR, model.select_mask
  );

  if (key.shift) {
    model.curr_pos = this->get_snapping_point(pos, model.rect.pos, model.scale);
  }

  // Draw the final line
  utils::draw_line(
      &model.img, *model.tex1, model.anim.get_size(), this->origin,
      model.curr_pos, model.color, model.select_mask
  );
}

} // namespace tool

