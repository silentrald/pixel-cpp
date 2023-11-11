/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-04
 *==========================*/

#include "./frame.hpp"
#include <cassert>

namespace draw {

Frame::Frame(usize* ptr, usize layer_count, usize id) noexcept
    : ptr(ptr), layer_count(layer_count), id(id) {}

u32 Frame::get_id() const noexcept {
  return this->id;
}

u32 Frame::get_image_id(usize index) const noexcept {
  assert(this->ptr != nullptr);
  assert(index >= 0U && index < this->layer_count);

  return this->ptr[index];
}

i32 Frame::get_layer_count() const noexcept {
  return this->layer_count;
}

} // namespace draw

