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
  explicit Image(data_ptr ptr, ivec size, ColorType type, u32 id) noexcept;

  [[nodiscard]] data_ptr get_ptr() const noexcept;
  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;
  [[nodiscard]] ColorType get_type() const noexcept;
  [[nodiscard]] u32 get_id() const noexcept;

  [[nodiscard]] data_ptr get_pixel(ivec pos) const noexcept;
  [[nodiscard]] data_ptr get_pixel(i32 index) const noexcept;

  void paint(ivec pos, rgba8 color) noexcept;
  void paint(i32 index, rgba8 color) noexcept;

private:
  data_ptr ptr = nullptr;
  ivec size{};
  ColorType type = ColorType::NONE;
  u32 id = 0U;
};

} // namespace draw

#endif

