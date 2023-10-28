/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-07
 *==========================*/

#include "./math.hpp"
#include <cassert>

i32 math::get_next_pow2(i32 num) noexcept {
  // Should only work for positive numbers
  assert(num >= 0);

  num |= num >> 0b00001;
  num |= num >> 0b00010;
  num |= num >> 0b00100;
  num |= num >> 0b01000;
  num |= num >> 0b10000;
  return num + 1;
}

bool math::is_nearly_equal(f32 f1, f32 f2) noexcept {
  return std::fabs(f1 - f2) < EPSILON;
}

