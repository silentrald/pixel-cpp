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

namespace cfg {

constexpr u32 hash_shortcut_string(const c8* str) noexcept {
  u32 out = 0U;
  for (i32 i = 0; i < 4; ++i) {
    if (str[i] == '\0') {
      break;
    }

    out |= str[i] << (i * 4);
  }
  return out;
}

enum class ShortcutKey {
  NONE = 0U,

  TOOL_PENCIL = hash_shortcut_string("pencil"),
  TOOL_ERASER = hash_shortcut_string("eraser"),
  TOOL_FILL = hash_shortcut_string("fill"),
  TOOL_LINE = hash_shortcut_string("line"),
  TOOL_SELECT = hash_shortcut_string("select"),

  ACTION_SAVE = hash_shortcut_string("save"),
  ACTION_UNDO = hash_shortcut_string("undo"),
  ACTION_REDO = hash_shortcut_string("redo"),
  ACTION_UNSELECT = hash_shortcut_string("unselect"),
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
  void load_config(const c8* path) noexcept;

  /**
   * Saves the current mapping to a file, from the path.
   * Recommeded to save as a .cfg file
   **/
  void save_config(const c8* path) const noexcept;

  [[nodiscard]] ShortcutKey get_shortcut_key(u32 key) const noexcept;

private:
  std::unordered_map<u32, ShortcutKey> map{};
};

} // namespace cfg

#endif

