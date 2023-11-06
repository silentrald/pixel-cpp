/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-30
 *==========================*/

#include "./image.hpp"
#include <cassert>

namespace draw {

Image::Image(data_ptr ptr, ivec size, ColorType type, u32 id) noexcept
    : ptr(ptr), size(size), type(type), id(id) {}

data_ptr Image::get_ptr() const noexcept {
  return this->ptr;
}

ivec Image::get_size() const noexcept {
  return this->size;
}

i32 Image::get_width() const noexcept {
  return this->size.x;
}

i32 Image::get_height() const noexcept {
  return this->size.y;
}

ColorType Image::get_type() const noexcept {
  return this->type;
}

u32 Image::get_id() const noexcept {
  return this->id;
}

data_ptr Image::get_pixel(ivec pos) const noexcept {
  assert(this->ptr != nullptr);
  assert(
      pos.x >= 0 && pos.y >= 0 && pos.x < this->size.x && pos.y < this->size.y
  );
  // NOLINTNEXTLINE
  return this->ptr + pos.x * pos.y * get_color_type_size(this->type);
}

data_ptr Image::get_pixel(i32 index) const noexcept {
  assert(this->ptr != nullptr);
  assert(index >= 0 && index < this->size.x * this->size.y);
  // NOLINTNEXTLINE
  return this->ptr + index * get_color_type_size(this->type);
}

void Image::paint(ivec pos, rgba8 color) noexcept {
  assert(this->ptr != nullptr);
  assert(
      pos.x >= 0 && pos.y >= 0 && pos.x < this->size.x && pos.y < this->size.y
  );
  // NOLINTNEXTLINE
  ((rgba8*)this->ptr)[pos.x * pos.y] = color;
}

void Image::paint(i32 index, rgba8 color) noexcept {
  assert(index >= 0 && index < this->size.x * this->size.y);
  ((rgba8*)this->ptr)[index] = color;
}

Image::operator bool() const noexcept {
  return this->ptr != nullptr;
}

} // namespace draw
