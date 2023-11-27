/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-20
 *==========================*/

#ifndef PXL_CFG_SHORTCUT_HPP
#define PXL_CFG_SHORTCUT_HPP

#include "types.hpp"
#include "view/input.hpp"
#include <unordered_map>

#define TEXT_ID(prefix, key) prefix##_##key = hash_shortcut(#key)

namespace cfg {

constexpr u32 hash_shortcut(const c8* str) noexcept {
  u32 out = 0U;
  for (i32 i = 0; i < 4; ++i) {
    if (str[i] == '\0') {
      break;
    }
    out = (out << 8) | str[i];
  }
  return out;
}

enum class ShortcutKey : u32 {
  NONE = 0U,

  TEXT_ID(TOOL, PENCIL),
  TEXT_ID(TOOL, ERASER),
  TEXT_ID(TOOL, FILL),
  TEXT_ID(TOOL, LINE),
  TEXT_ID(TOOL, SELECT),

  TEXT_ID(ACTION, SAVE),
  TEXT_ID(ACTION, OPEN),
  TEXT_ID(ACTION, UNDO),
  TEXT_ID(ACTION, REDO),
  TEXT_ID(ACTION, UNSELECT),
};

class Shortcut {
public:
  /**
   * Loads the file and parses it to this map
   *
   * Errors:
   *   - Reading a non-existing file
   *   - Reading a non-config file
   **/
  [[nodiscard]] error_code load_config(const c8* path) noexcept;

  /**
   * Saves the current mapping to a file, from the path.
   * Recommeded to save as a .cfg file
   **/
  [[nodiscard]] error_code save_config(const c8* path) const noexcept;

  [[nodiscard]] ShortcutKey get_shortcut_key(u32 key) const noexcept;

private:
  std::unordered_map<u32, ShortcutKey> map{};
};

} // namespace cfg

#undef TEXT_ID

#endif

