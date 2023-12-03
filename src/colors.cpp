/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-05
 *==========================*/

#include "types.hpp"
#include <cassert>

const c8* const HEX_VALUES = "0123456789abcdef";

std::string color::to_hex_string(rgba8 color) noexcept {
  std::string str{};
  str.resize(10);

  str[0] = '#';
  str[1] = HEX_VALUES[(color.r & 0xf0) >> 4];
  str[2] = HEX_VALUES[color.r & 0x0f];
  str[3] = HEX_VALUES[(color.g & 0xf0) >> 4];
  str[4] = HEX_VALUES[color.g & 0x0f];
  str[5] = HEX_VALUES[(color.b & 0xf0) >> 4];
  str[6] = HEX_VALUES[color.b & 0x0f];
  str[7] = HEX_VALUES[(color.a & 0xf0) >> 4];
  str[8] = HEX_VALUES[color.a & 0x0f];

  return str;
}

u8 parse_char(c8 c) noexcept {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }

  if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10U;
  }

  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10U;
  }

  return 0U;
}

rgba8 color::parse_hex_string(const c8* hex) noexcept {
  if ((hex[7] != '\0' && hex[9] != '\0') || hex[0] != '#') {
    return {};
  }

  rgba8 out{};
  out.r = (parse_char(hex[1]) << 4) | parse_char(hex[2]);
  out.g = (parse_char(hex[3]) << 4) | parse_char(hex[4]);
  out.b = (parse_char(hex[5]) << 4) | parse_char(hex[6]);
  out.a =
      hex[7] == '\0' ? 0xff : (parse_char(hex[7]) << 4) | parse_char(hex[8]);
  return out;
}

