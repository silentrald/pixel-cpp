/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-06
 *==========================*/

#include "./frame.hpp"

namespace draw {

Frame::Frame(data_ptr ptr, i32 next_layer, ivec size, ColorType type) noexcept
    : ptr(ptr), next_layer(next_layer), size(size), type(type) {}

ivec Frame::get_size() const noexcept {
  return this->size;
}

i32 Frame::get_width() const noexcept {
  return this->size.x;
}

i32 Frame::get_height() const noexcept {
  return this->size.y;
}

ColorType Frame::get_type() const noexcept {
  return this->type;
}

Layer Frame::get_layer(i32 index) noexcept {
  assert(index >= 0 && index < this->layer_count);

  // NOLINTNEXTLINE
  return Layer{this->ptr + index * this->next_layer, this->size, this->type};
}

} // namespace draw
