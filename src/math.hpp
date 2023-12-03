/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-07
 *==========================*/

#ifndef PXL_MATH_HPP
#define PXL_MATH_HPP

#include "types.hpp"
#include <complex>

namespace math {

inline const f32 EPSILON = 0.000001F;
inline const f32 PI = 3.142857F;

/**
 * Returns the power of 2 that is bigger than the given number.
 * eg. 2 -> 4; 3 -> 4; 5 -> 8; 9 -> 16
 */
u32 get_next_pow2(u32 num) noexcept;

/**
 * Returns the power of 2 that is bigger than the given number.
 * eg. 2 -> 4; 3 -> 4; 5 -> 8; 9 -> 16
 */
u64 get_next_pow2(u64 num) noexcept;

[[nodiscard]] bool is_nearly_equal(f32 f1, f32 f2) noexcept;

/**
 * Normalizes the u8 datatype to a value from 0.0F to 1.0F
 **/
[[nodiscard]] f32 normalize(u8 num) noexcept;

} // namespace math

#endif

