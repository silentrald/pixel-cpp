/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-07
 *==========================*/

#include "./math.hpp"
#include <cassert>

u32 math::get_next_pow2(u32 num) noexcept {
  num |= num >> 0b00001;
  num |= num >> 0b00010;
  num |= num >> 0b00100;
  num |= num >> 0b01000;
  num |= num >> 0b10000;

  // Don't overflow, just return the UINT32_MAX value
  return num == UINT32_MAX ? num : num + 1U;
}

u64 math::get_next_pow2(u64 num) noexcept {
  num |= num >> 0b000001;
  num |= num >> 0b000010;
  num |= num >> 0b000100;
  num |= num >> 0b001000;
  num |= num >> 0b010000;
  num |= num >> 0b100000;

  // Don't overflow, just return the UINT64_MAX value
  return num == UINT64_MAX ? num : num + 1LU;
}

bool math::is_nearly_equal(f32 f1, f32 f2) noexcept {
  return std::fabs(f1 - f2) < EPSILON;
}

f32 math::normalize(u8 num) noexcept {
  return num * (1.0F / 255.0F);
}

