/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-06
 *==========================*/

#include "./layer.hpp"

namespace draw {

Layer::Layer(data_ptr ptr, ivec size, ColorType type) noexcept
    : ptr(ptr), size(size), type(type) {}

data_ptr Layer::get_ptr() noexcept {
  return this->ptr;
}

ivec Layer::get_size() const noexcept {
  return this->size;
}

i32 Layer::get_width() const noexcept {
  return this->size.x;
}

i32 Layer::get_height() const noexcept {
  return this->size.y;
}

ColorType Layer::get_type() const noexcept {
  return this->type;
}

data_ptr Layer::get_pixel(ivec pos) const noexcept {
  // NOLINTNEXTLINE
  return this->ptr +
         (pos.x + pos.y * this->size.x) * (this->type & 0x0000'ffff);
}

data_ptr Layer::get_pixel(i32 index) const noexcept {
  // NOLINTNEXTLINE
  return this->ptr + index * (this->type & 0x0000'ffff);
}

void Layer::paint(ivec pos, rgba8 color) noexcept {
  assert(this->ptr != nullptr);
  assert((this->type & 0x0000'ffff) == sizeof(rgba8));
  assert(
      pos.x >= 0 && pos.y >= 0 && pos.x < this->size.x && pos.y < this->size.y
  );

  ((rgba8*)this->ptr)[pos.x + pos.y * this->size.x] = color;
}

void Layer::paint(i32 index, rgba8 color) noexcept {
  assert(this->ptr != nullptr);
  assert((this->type & 0x0000'ffff) == sizeof(rgba8));
  assert(index >= 0 && index < this->size.x * this->size.y);

  ((rgba8*)this->ptr)[index] = color;
}

} // namespace draw

