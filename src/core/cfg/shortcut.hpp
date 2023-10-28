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

enum class ShortcutKey {
  NONE,

  TOOL_ERASER,
  TOOL_FILL,
  TOOL_LINE,
  TOOL_PENCIL,
  TOOL_SELECT,

  ACTION_UNDO,
  ACTION_REDO,
  ACTION_UNSELECT,
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

