/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-09-14
 *===============================*/

#include "./select.hpp"
#include <algorithm>

namespace tool {

u32 Select::execute(Model& model, const event::Input& evt) noexcept {
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

// TODO: Can still be optimized
// BUG: Select doesn't update texture in mouse_down
void Select::handle_mouse_down(Model& model, fvec pos) noexcept {
  this->origin = {
      .x = std::clamp(0, model.anim.get_width() - 1, model.curr_pos.x),
      .y = std::clamp(0, model.anim.get_height() - 1, model.curr_pos.y),
  };

  std::fill(model.select_mask.begin(), model.select_mask.end(), false);
  if (model.get_pixel_index()) {
    model.select_mask[model.get_pixel_index()] = true;
  }

  if (this->size == model.anim.get_size()) {
    return;
  }
  this->size = model.anim.get_size();
  i32 _size = model.anim.get_width() * model.anim.get_height();
  this->outline_mask.resize(_size);

  for (i32 i = 0; i < this->pixels.size(); ++i) {
    auto& p = this->pixels[i];
    p.resize(_size);
    for (i32 y = 0; y < model.anim.get_height(); ++y) {
      for (i32 x = 0; x < model.anim.get_width(); ++x) {
        p[x + y * model.anim.get_width()] =
            (x ^ y ^ i) & 1 ? rgba8{0xee, 0xee, 0xee} : rgba8{};
      }
    }
  }
}

// NOTE: Selecting the corner of the canvas, seems janky
/**
 * Uses:
 *   model.tex1 - select texture 1
 *   model.tex2 - select texture 2
 **/
void Select::handle_mouse_motion(Model& model, fvec pos) noexcept {
  if (this->pixels[0].empty() || model.curr_pos == model.prev_pos) {
    return;
  }

  std::fill(model.select_mask.begin(), model.select_mask.end(), false);

  ivec start{};
  ivec end{};

  // Switch values
  if (this->origin.x <= model.curr_pos.x) {
    start.x = this->origin.x;
    end.x = model.curr_pos.x;
  } else {
    start.x = model.curr_pos.x;
    end.x = this->origin.x;
  }

  if (this->origin.y <= model.curr_pos.y) {
    start.y = this->origin.y;
    end.y = model.curr_pos.y;
  } else {
    start.y = model.curr_pos.y;
    end.y = this->origin.y;
  }

  // Clamping
  start.x = std::max(0, start.x);
  start.y = std::max(0, start.y);
  end.x = std::min(end.x, model.anim.get_width() - 1);
  end.y = std::min(end.y, model.anim.get_height() - 1);

  for (i32 y = start.y; y <= end.y; ++y) {
    for (i32 x = start.x; x <= end.x; ++x) {
      model.select_mask[x + y * model.anim.get_width()] = true;
    }
  }

  // Outline algorithm
  std::fill(this->outline_mask.begin(), this->outline_mask.end(), false);

  // top
  i32 e = 0;
  if (start.y > 0) {
    e = std::min(end.x + 1, model.anim.get_width() - 1) +
        (start.y - 1) * model.anim.get_width();
    for (i32 i =
             std::max(0, start.x - 1) + (start.y - 1) * model.anim.get_width();
         i <= e; ++i) {
      this->outline_mask[i] = true;
    }
  }

  // bot
  if (end.y < model.anim.get_height() - 1) {
    e = std::min(end.x + 1, model.anim.get_width() - 1) +
        (end.y + 1) * model.anim.get_width();
    for (i32 i =
             std::max(0, start.x - 1) + (end.y + 1) * model.anim.get_width();
         i <= e; ++i) {
      this->outline_mask[i] = true;
    }
  }

  // left
  if (start.x > 0) {
    e = start.x - 1 +
        std::min(end.y + 1, model.anim.get_height() - 1) *
            model.anim.get_width();
    for (i32 i =
             start.x - 1 + std::max(0, start.y - 1) * model.anim.get_width();
         i <= e; i += model.anim.get_width()) {
      this->outline_mask[i] = true;
    }
  }

  // right
  if (end.x < model.anim.get_width() - 1) {
    e = end.x + 1 +
        std::min(end.y + 1, model.anim.get_height() - 1) *
            model.anim.get_width();
    for (i32 i = end.x + 1 + std::max(0, start.y - 1) * model.anim.get_width();
         i <= e; i += model.anim.get_width()) {
      this->outline_mask[i] = true;
    }
  }

  // Draw on the texture
  auto& p0 = this->pixels[0];
  for (i32 i = 0; i < outline_mask.size(); ++i) {
    p0[i].a = outline_mask[i] * 0xff;
  }
  model.tex1->set_pixels(p0.data(), model.anim.get_size());

  auto& p1 = this->pixels[1];
  for (i32 i = 0; i < outline_mask.size(); ++i) {
    p1[i].a = outline_mask[i] * 0xff;
  }
  model.tex2->set_pixels(p1.data(), model.anim.get_size());
}

} // namespace tool
