/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-06
 *==========================*/

#ifndef PXL_DRAW_TYPES_HPP
#define PXL_DRAW_TYPES_HPP

#include "../../types.hpp"

namespace draw {

// last 4 bytes is the size compares to char*
enum ColorType : i32 {
  NONE = 0,
  RGBA8 = 0x0001'0004,
  RGBA16 = 0x0002'0008,
};

i32 get_color_type_size(ColorType type) noexcept;

using data_ptr = u8*;

} // namespace draw

#endif

