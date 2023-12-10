/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#ifndef PXL_CFG_LANG_HPP
#define PXL_CFG_LANG_HPP

#define TEXT_ID(section, key) key = hash_locale(#key) | ((u32)section << 16)

#include "types.hpp"

namespace cfg::locale {

enum class Locale : u32 { ENGLISH, JAPANESE };

constexpr u32 hash_locale(const c8* str) noexcept {
  assert(str[0] != '\0');
  return str[0] | (str[1] << 8);
}

// Occupies the first 16 bits of the text id
enum Section : u16 {
  GENERAL = 0U,
  MENU_ITEM,
  FILE_CTX_MENU,
  EXPORT_CTX_MENU,
  EDIT_CTX_MENU,
  LAYERS_CTX_MENU,
  TIMELINE_CTX_MENU,
  MODAL_TITLE,
  SIZE // Last Elem
};

enum TextId : u32 {
  // General
  TEXT_ID(GENERAL, WIDTH),
  TEXT_ID(GENERAL, HEIGHT),
  TEXT_ID(GENERAL, PX),
  TEXT_ID(GENERAL, CANCEL),
  TEXT_ID(GENERAL, LAYERS),
  TEXT_ID(GENERAL, PROPERTIES),

  // Menu Items
  TEXT_ID(MENU_ITEM, FILE_),
  TEXT_ID(MENU_ITEM, EDIT),
  TEXT_ID(MENU_ITEM, VIEW),

  // File Context Menu
  TEXT_ID(FILE_CTX_MENU, NEW),
  TEXT_ID(FILE_CTX_MENU, OPEN),
  TEXT_ID(FILE_CTX_MENU, SAVE),
  TEXT_ID(FILE_CTX_MENU, SV_AS),
  TEXT_ID(FILE_CTX_MENU, EXPORT),

  // Export Context Menu
  TEXT_ID(EXPORT_CTX_MENU, PNG_EXPORT),
  TEXT_ID(EXPORT_CTX_MENU, JPG_EXPORT),

  // Edit Context Menu
  TEXT_ID(EDIT_CTX_MENU, UNDO),
  TEXT_ID(EDIT_CTX_MENU, REDO),

  // Layers Context Menu
  TEXT_ID(LAYERS_CTX_MENU, INS_LAYER),
  TEXT_ID(LAYERS_CTX_MENU, REM_LAYER),

  // Timeline Context Menu
  TEXT_ID(TIMELINE_CTX_MENU, BLANK_FRAME_INS),
  TEXT_ID(TIMELINE_CTX_MENU, REM_FRAME),

  // Modal Titles
  TEXT_ID(MODAL_TITLE, NEW_FILE),
};

[[nodiscard]] error_code load_locale(Locale locale) noexcept;
void destroy_locale() noexcept;

const c8* get_font() noexcept;

i32 get_size() noexcept;

const c8* get_text(TextId id) noexcept;

} // namespace cfg::locale

#undef TEXT_ID

#endif

