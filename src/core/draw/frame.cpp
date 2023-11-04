/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-04
 *==========================*/

#include "./frame.hpp"
#include <cassert>

namespace draw {

Frame::Frame(u32* ptr, i32 layer_count, u32 id) noexcept
    : ptr(ptr), layer_count(layer_count), id(id) {}

u32 Frame::get_id() const noexcept {
  return this->id;
}

u32 Frame::get_image_id(i32 index) const noexcept {
  assert(this->ptr != nullptr);
  assert(index >= 0 && index < this->layer_count);

  return this->ptr[index];
}

i32 Frame::get_layer_count() const noexcept {
  return this->layer_count;
}

} // namespace draw

