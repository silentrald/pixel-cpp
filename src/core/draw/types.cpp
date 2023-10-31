/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-30
 *==========================*/

#include "./types.hpp"

i32 draw::get_color_type_size(ColorType type) noexcept {
  return 0x0000'ffff & type;
}

