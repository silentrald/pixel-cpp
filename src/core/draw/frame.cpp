/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-04
 *==========================*/

#include "./frame.hpp"
#include <cassert>

namespace draw {

Frame::Frame(usize* ptr, usize layer_count, usize index) noexcept
    : ptr(ptr), layer_count(layer_count), index(index) {}

usize Frame::get_index() const noexcept {
  return this->index;
}

usize Frame::get_image_id(usize index) const noexcept {
  assert(this->ptr != nullptr);
  assert(index < this->layer_count);

  return this->ptr[index];
}

usize Frame::get_layer_count() const noexcept {
  return this->layer_count;
}

} // namespace draw

