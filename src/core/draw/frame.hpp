/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-06
 *==========================*/

#ifndef PXL_DRAW_FRAME_HPP
#define PXL_DRAW_FRAME_HPP

#include "./layer.hpp"
#include "./types.hpp"
#include "types.hpp"
#include <cassert>

namespace draw {

/**
 * References a frame from an animation.
 * Collection of layers.
 *
 * NOTE: This may get invalidated if the main animation object is mutated
 **/
class Frame {
public:
  Frame() noexcept = default;

  explicit Frame(
      data_ptr ptr, i32 next_layer, ivec size, ColorType type
  ) noexcept;

  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;

  // Returns the type, with the size information of bytes
  [[nodiscard]] ColorType get_type() const noexcept;

  [[nodiscard]] Layer get_layer(i32 index) noexcept;

private:
  data_ptr ptr = nullptr;
  i32 next_layer = 0;
  i32 layer_count = 0;
  ivec size{};
  ColorType type = ColorType::NONE;
};

} // namespace draw

#endif

