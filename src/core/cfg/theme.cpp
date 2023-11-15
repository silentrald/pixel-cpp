/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-22
 *==========================*/

#include "./theme.hpp"
#include "core/logger/logger.hpp"

namespace cfg {

// NOTE: Refer indexing to input::BtnState in input.hpp

// NOLINTNEXTLINE
rgba8 menu_btn_colors[] = {
    {0x00, 0x00, 0x00, 0xff},
    {0x88, 0x88, 0x88, 0xff},
    {0xcc, 0xcc, 0xcc, 0xff},
    {0x44, 0x44, 0x44, 0xff}};

// NOLINTNEXTLINE
rgba8 tool_btn_colors[] = {
    {0x00, 0x00, 0x00, 0xff},
    {0x88, 0x88, 0x88, 0xff},
    {0xcc, 0xcc, 0xcc, 0xff},
    {0x44, 0x44, 0x44, 0xff}};

error_code theme::load_config(const c8* path) noexcept {
  // TODO: Loader plus hex string to rgba8 function helper
  logger::fatal("Not yet implemented UwU");
  std::abort();
}

rgba8 theme::get_menu_btn_color(input::BtnState state) noexcept {
  return menu_btn_colors[state];
}

inline rgba8 get_color(const rgba8* colors, u32 info) noexcept {
  if (info & input::BtnMask::HIGHLIGHT) {
    return colors[input::BtnState::HOVER];
  }

  return colors[info & input::BtnMask::STATES];
}

rgba8 theme::get_button_color(u32 info) noexcept {
  switch (info & input::BtnMask::THEME) {
  case input::BtnTheme::TOOL_BTN:
    return get_color(tool_btn_colors, info);

  case input::BtnTheme::CUSTOM:
    // TODO:
    return {0xff, 0xff, 0xff, 0xff};

  default:
    return {};
  }
}

} // namespace cfg

