/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-22
 *==========================*/

#ifndef PXL_CFG_THEME_HPP
#define PXL_CFG_THEME_HPP

#include "types.hpp"
#include "view/input.hpp"

namespace cfg::theme {

[[nodiscard]] error_code load_config(const c8* path) noexcept;

rgba8 get_menu_btn_color(input::BtnState state) noexcept;

rgba8 get_button_color(u32 info) noexcept;

} // namespace cfg::theme

#endif

