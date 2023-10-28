/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-06
 *==========================*/

#ifndef PXL_DRAW_LAYER_HPP
#define PXL_DRAW_LAYER_HPP

#include "./types.hpp"
#include "types.hpp"
#include <cassert>

namespace draw {

/**
 * References a layer from an animation
 * This contains of unsafe functions to paint
 *
 * NOTE: This may get invalidated if the main animation object is mutated
 **/
class Layer {
public:
  Layer() noexcept = default;

  explicit Layer(data_ptr ptr, ivec size, ColorType type) noexcept;

  /**
   * Returns the ptr which points at the start of the layer.
   * data_ptr can be converted to any pointer type, but use get_type()
   * to check whether what datatype was used.
   **/
  [[nodiscard]] data_ptr get_ptr() noexcept;

  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;

  // Returns the type, with the size information of bytes
  [[nodiscard]] ColorType get_type() const noexcept;

  // Returns the pixel at a specific pos
  [[nodiscard]] data_ptr get_pixel(ivec pos) const noexcept;

  // Returns the pixel at a specific index
  [[nodiscard]] data_ptr get_pixel(i32 index) const noexcept;

  void paint(ivec pos, rgba8 color) noexcept;
  void paint(i32 index, rgba8 color) noexcept;

private:
  data_ptr ptr = nullptr;
  ivec size{};
  ColorType type = ColorType::NONE;
};

} // namespace draw

#endif

