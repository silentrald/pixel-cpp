/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-30
 *==========================*/

#ifndef PXL_DRAW_IMAGE_HPP
#define PXL_DRAW_IMAGE_HPP

#include "./types.hpp"
#include "types.hpp"

namespace draw {

class Image {
public:
  Image() noexcept = default;
  explicit Image(data_ptr pixels, ivec size, ColorType type, usize id) noexcept;

  [[nodiscard]] data_ptr get_pixels() const noexcept;
  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;
  [[nodiscard]] ColorType get_type() const noexcept;
  [[nodiscard]] usize get_id() const noexcept;

  [[nodiscard]] data_ptr get_pixel(ivec pos) const noexcept;
  [[nodiscard]] data_ptr get_pixel(i32 index) const noexcept;

  void set_pixels(data_ptr pixels, usize size) noexcept;
  void paint(ivec pos, rgba8 color) noexcept;
  void paint(usize index, rgba8 color) noexcept;

  [[nodiscard]] explicit operator bool() const noexcept;

#ifndef NDEBUG

  void print() const noexcept;

#endif

private:
  data_ptr pixels = nullptr;
  ivec size{};
  ColorType type = ColorType::NONE;
  usize id = 0U;
};

} // namespace draw

#endif

