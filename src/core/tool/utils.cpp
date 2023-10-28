/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#include "./utils.hpp"

namespace tool::utils {

// === Line Drawing Algorithm === //
// Bresenham's Algo
void draw_horizontal_line(
    draw::Layer* layer, Texture& texture, ivec size, i32 start_x, i32 end_x,
    i32 y, rgba8 color, const std::vector<bool>& mask
) noexcept {
  // Check if the line is within bounds
  if (y < 0 || y >= size.y) {
    return;
  }

  if (start_x > end_x) {
    std::swap(start_x, end_x);
  }

  // Check if the line is within bounds
  if (end_x < 0 || start_x >= size.x) {
    return;
  }

  start_x = std::max(0, start_x) + y * size.x;
  end_x = std::min(size.x - 1, end_x) + y * size.x;

  auto pixels = texture.lock_texture<rgba8>();
  for (i32 i = start_x; i <= end_x; ++i) {
    if (!mask[i]) {
      continue;
    }

    pixels.paint(i, color);
    if (layer)
      layer->paint(i, color);
  }
}

void draw_vertical_line(
    draw::Layer* layer, Texture& texture, ivec size, i32 start_y, i32 end_y,
    i32 x, rgba8 color, const std::vector<bool>& mask
) noexcept {
  // Check if the line is within bounds
  if (x < 0 || x >= size.x) {
    return;
  }

  if (start_y > end_y) {
    std::swap(start_y, end_y);
  }

  // Check if the line is within bounds
  if (end_y < 0 || start_y >= size.y) {
    return;
  }

  start_y = std::max(0, start_y) * size.x + x;
  end_y = std::min(size.y - 1, end_y) * size.x + x;

  auto pixels = texture.lock_texture<rgba8>();
  for (i32 i = start_y; i <= end_y; i += size.x) {
    if (!mask[i]) {
      continue;
    }

    pixels.paint(i, color);
    if (layer)
      layer->paint(i, color);
  }
}

void draw_line_low(
    draw::Layer* layer, Texture& texture, ivec size, ivec start, ivec end,
    rgba8 color, const std::vector<bool>& mask
) noexcept {
  ivec d = end - start;

  i32 yi = 1;
  if (d.y < 0) {
    yi = -1;
    d.y = -d.y;
  }

  i32 error = (2 * d.y) - d.x;
  i32 x = start.x;
  i32 y = start.y;

  end.x = std::min(size.x - 1, end.x);
  for (; x < 0; ++x) {
    if (error > 0) {
      y += yi;
      error += 2 * (d.y - d.x);
    } else {
      error += 2 * d.y;
    }
  }

  auto pixels = texture.lock_texture<rgba8>();
  for (; x <= end.x; ++x) {
    if (y >= 0 && y < size.y && mask[x + y * size.x]) {
      pixels.paint(x + y * size.x, color);
      if (layer)
        layer->paint(x + y * size.x, color);
    }

    if (error > 0) {
      y += yi;
      error += 2 * (d.y - d.x);
    } else {
      error += 2 * d.y;
    }
  }
}

void draw_line_high(
    draw::Layer* layer, Texture& texture, ivec size, ivec start, ivec end,
    rgba8 color, const std::vector<bool>& mask
) noexcept {
  ivec d = end - start;

  i32 xi = 1;
  if (d.x < 0) {
    xi = -1;
    d.x = -d.x;
  }

  i32 error = 2 * d.x - d.y;
  i32 x = start.x;
  i32 y = start.y;

  end.y = std::min(end.y, size.y - 1);
  for (; y < 0; ++y) {
    if (error > 0) {
      x += xi;
      error += 2 * (d.x - d.y);
    } else {
      error += 2 * d.x;
    }
  }

  auto pixels = texture.lock_texture<rgba8>();
  for (; y <= end.y; ++y) {
    if (x >= 0 && x < size.x && mask[x + y * size.x]) {
      pixels.paint(x + y * size.x, color);
      if (layer)
        layer->paint(x + y * size.x, color);
    }

    if (error > 0) {
      x += xi;
      error += 2 * (d.x - d.y);
    } else {
      error += 2 * d.x;
    }
  }
}

void draw_line(
    draw::Layer* layer, Texture& texture, ivec size, ivec start, ivec end,
    rgba8 color, const std::vector<bool>& mask
) noexcept {
  // TEST: Check if the line is within the rect
  // Unlikely to happen if user always draw in the canvas

  if (start.x == end.x) {
    draw_vertical_line(
        layer, texture, size, start.y, end.y, start.x, color, mask
    );
  } else if (start.y == end.y) {
    draw_horizontal_line(
        layer, texture, size, start.x, end.x, start.y, color, mask
    );
  }

  if (std::abs(start.y - end.y) < std::abs(start.x - end.x)) {
    if (start.x > end.x) {
      draw_line_low(layer, texture, size, end, start, color, mask);
    } else {
      draw_line_low(layer, texture, size, start, end, color, mask);
    }
  } else {
    if (start.y > end.y) {
      draw_line_high(layer, texture, size, end, start, color, mask);
    } else {
      draw_line_high(layer, texture, size, start, end, color, mask);
    }
  }
}

} // namespace tool::utils

