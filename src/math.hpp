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
i32 get_next_pow2(i32 num) noexcept;

[[nodiscard]] bool is_nearly_equal(f32 f1, f32 f2) noexcept;

} // namespace math

#endif

