/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-20
 *==========================*/

#include "./shortcut.hpp"
#include "core/logger/logger.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

namespace cfg {

inline bool compare_string(const c8* str1, const c8* str2) noexcept {
  return std::strncmp(str1, str2, std::strlen(str2)) == 0;
}

u32 convert_str_to_key(const c8* str) noexcept {
  u32 key = 0U;
  if (std::strncmp("ctrl+", str, 5) == 0) {
    key |= input::Keycode::CTRL;
    str += 5;
  }

  if (std::strncmp("shift+", str, 6) == 0) {
    key |= input::Keycode::SHIFT;
    str += 6;
  }

  if (std::strncmp("alt+", str, 4) == 0) {
    key |= input::Keycode::ALT;
    str += 4;
  }

  if (str[0] >= 'A' && str[0] <= 'Z') {
    return key | (str[0] - 'A' + 'a');
  }

  return key | str[0];
}

bool inline is_white_space(c8 c) noexcept {
  return c == ' ' || c == '\n';
}

i32 inline get_first_non_whitespace_index(
    const c8* str, i32 start = 0
) noexcept {
  while (str[start] != '\0' && is_white_space(str[start]))
    ++start;
  return start;
}

// Create test cases for this, might create a class for this
error_code Shortcut::load_config(const c8* path) noexcept {
  std::ifstream ifs{path};
  if (!ifs.is_open()) {
    logger::error("Could not read file %s", path);
    return error_code::FILE_NOT_FOUND;
  }

  std::string line{};
  ShortcutKey key_map{};
  std::size_t equal_index = 0;
  i32 left_cursor = 0;
  i32 right_cursor = 0;
  while (std::getline(ifs, line, '\n')) {
    // left trim
    left_cursor = get_first_non_whitespace_index(line.c_str(), 0);
    if (line[left_cursor] == '#' ||
        line[left_cursor] == '\0') { // Comment or Empty
      continue;
    }

    equal_index = line.find('=', left_cursor);
    if (equal_index == std::string::npos ||
        equal_index == 0U) { // Don't get anything here
      continue;
    }

    // Read the key_map value
    // right trim
    right_cursor = equal_index - 1;
    while (right_cursor > 0 && is_white_space(right_cursor))
      --right_cursor;
    line[right_cursor] = '\0';
    key_map = (ShortcutKey)hash_shortcut(line.c_str() + left_cursor);
    if (key_map == ShortcutKey::NONE) {
      continue;
    }

    // Read the key
    left_cursor = get_first_non_whitespace_index(line.c_str(), equal_index + 1);
    if (line[left_cursor] == '#' || line[left_cursor] == '\0') {
      continue;
    }
    equal_index = convert_str_to_key(line.c_str() + left_cursor);
    if (equal_index == 0U) {
      continue;
    }

    this->map.insert({equal_index, key_map});
  }

  ifs.close();

  return error_code::OK;
}

error_code Shortcut::save_config(const c8* path) const noexcept {
  // TODO:
  logger::fatal("Not yet implemented UwU");
  std::abort();
}

ShortcutKey Shortcut::get_shortcut_key(u32 key) const noexcept {
  auto it = this->map.find(key);
  return it == this->map.cend() ? ShortcutKey::NONE : it->second;
}

} // namespace cfg

