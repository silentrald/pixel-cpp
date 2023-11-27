/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-29
 *===============================*/

#include "./fill.hpp"
#include <cstdio>
#include <stack>

namespace tool {

/**
 * Uses:
 *   model.tex1 - current image
 **/
u32 Fill::execute(Model& model, const event::Input& evt) noexcept {
  if ((evt.mouse.left != input::MouseState::UP &&
       evt.mouse.right != input::MouseState::UP) ||
      !model.anim.has_point(model.curr_pos) ||
      !model.select_mask[model.get_pixel_index()]) {
    return event::Flag::NONE;
  }

  this->new_color =
      evt.mouse.left == input::MouseState::UP ? model.fg_color : model.bg_color;
  this->old_color = *(rgba8*)model.img.get_pixel(model.get_pixel_index());

  if (this->old_color == this->new_color) {
    return event::Flag::NONE;
  }

  this->scan_fill(
      model.img, *model.tex1, model.anim.get_size(), model.curr_pos,
      model.select_mask
  );
  return event::Flag::SNAPSHOT;
}

[[nodiscard]] inline i32 get_index(ivec pos, i32 width) noexcept {
  return pos.x + pos.y * width;
}

bool Fill::check_pixel(
    const std::vector<bool>& mask, const draw::Image& image, ivec pos, i32 width
) const noexcept {
  return mask[get_index(pos, width)] &&
         *(rgba8*)image.get_pixel(get_index(pos, width)) == this->old_color;
}

void Fill::scan_fill(
    draw::Image& image, Texture& texture, ivec size, ivec pos,
    const std::vector<bool>& mask
) noexcept {
  std::stack<ivec> s{};
  bool span_above = false;
  bool span_below = false;

  s.push(pos);

  auto pixels = texture.lock_texture<rgba8>();
  while (!s.empty()) {
    pos = s.top();
    s.pop();

    while (pos.x >= 0 && this->check_pixel(mask, image, pos, size.x)) {
      --pos.x;
    }
    ++pos.x;
    span_above = span_below = false;

    for (; pos.x < size.x && this->check_pixel(mask, image, pos, size.x);
         ++pos.x) {
      image.paint(get_index(pos, size.x), this->new_color);
      pixels.paint(get_index(pos, size.x), this->new_color);

      if (!span_above && pos.y > 0 &&
          this->check_pixel(mask, image, {pos.x, pos.y - 1}, size.x)) {
        s.push({pos.x, pos.y - 1});
        span_above = true;
      } else if (span_above && pos.y > 0 && !this->check_pixel(mask, image, {pos.x, pos.y - 1}, size.x)) {
        span_above = false;
      }

      if (!span_below && pos.y < size.y - 1 &&
          this->check_pixel(mask, image, {pos.x, pos.y + 1}, size.x)) {
        s.push({pos.x, pos.y + 1});
        span_below = true;
      } else if (span_below && pos.y < size.y - 1 && !this->check_pixel(mask, image, {pos.x, pos.y + 1}, size.x)) {
        span_below = false;
      }
    }
  }
}

} // namespace tool
