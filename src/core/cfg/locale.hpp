/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#ifndef PXL_CFG_LANG_HPP
#define PXL_CFG_LANG_HPP

#include "types.hpp"

namespace cfg::locale {

enum class Locale : u32 { ENGLISH, JAPANESE };

constexpr u32 hash_string(const c8* str) noexcept {
  u32 out = 0U;
  for (i32 i = 0; i < 4; ++i) {
    if (str[i] == '\0') {
      break;
    }

    out |= str[i] << (i * 4);
  }
  return out;
}

enum TextId : u32 {
  // General
  LAYERS = hash_string("layers"),
  WIDTH = hash_string("width"),
  HEIGHT = hash_string("height"),
  PX = hash_string("px"),
  NEW = hash_string("new"),
  CANCEL = hash_string("cancel"),

  // Menu Items
  FILE_MENU_ITEM = hash_string("file_menu_item"),
  EDIT_MENU_ITEM = hash_string("edit_menu_item"),
  VIEW_MENU_ITEM = hash_string("view_menu_item"),

  // Modal Titles
  NEW_FILE = hash_string("new_file"),
};

[[nodiscard]] error_code load_locale(Locale locale) noexcept;

const c8* get_font() noexcept;

i32 get_size() noexcept;

const c8* get_text(TextId id) noexcept;

} // namespace cfg::locale

#endif

