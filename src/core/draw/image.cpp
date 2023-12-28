/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-30
 *==========================*/

#include "./image.hpp"
#include "core/logger/logger.hpp"
#include <cassert>
#include <cstring>

namespace draw {

Image::Image(data_ptr pixels, ivec size, ColorType type, usize id) noexcept
    : pixels(pixels), size(size), type(type), id(id) {}

data_ptr Image::get_pixels() const noexcept {
  return this->pixels;
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

usize Image::get_id() const noexcept {
  return this->id;
}

data_ptr Image::get_pixel(ivec pos) const noexcept {
  assert(this->pixels != nullptr);
  assert(
      pos.x >= 0 && pos.y >= 0 && pos.x < this->size.x && pos.y < this->size.y
  );
  // NOLINTNEXTLINE
  return this->pixels + pos.x * pos.y * get_color_type_size(this->type);
}

data_ptr Image::get_pixel(i32 index) const noexcept {
  assert(this->pixels != nullptr);
  assert(index >= 0 && index < this->size.x * this->size.y);
  // NOLINTNEXTLINE
  return this->pixels + index * get_color_type_size(this->type);
}

void Image::set_pixels(data_ptr pixels, usize size) noexcept {
  assert(size == this->size.x * this->size.y * get_color_type_size(this->type));
  std::memcpy(this->pixels, pixels, size);
}

void Image::paint(ivec pos, rgba8 color) noexcept {
  assert(this->pixels != nullptr);
  assert(
      pos.x >= 0 && pos.y >= 0 && pos.x < this->size.x && pos.y < this->size.y
  );
  // NOLINTNEXTLINE
  reinterpret_cast<rgba8*>(this->pixels)[pos.x * pos.y] = color;
}

void Image::paint(usize index, rgba8 color) noexcept {
  assert(index < this->size.x * this->size.y);
  reinterpret_cast<rgba8*>(this->pixels)[index] = color;
}

Image::operator bool() const noexcept {
  return this->pixels != nullptr;
}

#ifndef NDEBUG

void Image::print(bool lock) const noexcept {
  LOCK_PRINT(logger::Level::DEBUG_LVL, "Image Info", lock);

  logger::print(
      "  Image Id: " USIZE_FMT " ; Size (%d, %d) ; Ptr: %p", this->id,
      this->size.x, this->size.y, this->pixels
  );

  usize mod = this->size.x * get_color_type_size(this->type);
  usize psize = mod * this->size.y;
  for (usize i = 0U; i < psize; ++i) {
    if (i % mod == 0U) {
      logger::print("\n");
    }
    logger::print("%02x", this->pixels[i]);
  }
  logger::print("\n");

  UNLOCK_PRINT(lock);
}

#endif

} // namespace draw
