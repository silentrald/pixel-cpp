/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#include "./locale.hpp"
#include "core/logger/logger.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>

namespace cfg {

namespace locale {

std::unordered_map<TextId, std::string> texts{};
Locale _locale{};

} // namespace locale

inline bool compare_string(const c8* str1, const c8* str2) noexcept {
  return std::strncmp(str1, str2, std::strlen(str2)) == 0;
}

// Hash
locale::TextId parse_key(const c8* key) noexcept {
  u32 out = 0U;
  for (i32 i = 0; i < 4; ++i) {
    if (key[i] == '\0') {
      break;
    }

    out |= key[i] << (i * 4);
  }
  return (locale::TextId)out;
}

error_code locale::load_locale(Locale locale) noexcept {
  _locale = locale;

  texts.clear();

  const c8* locale_file = nullptr;
  switch (locale) {
  case Locale::ENGLISH:
    locale_file = "locale/en.cfg";
    break;

  case Locale::JAPANESE:
    locale_file = "locale/jp.cfg";
    break;

  default:
    std::abort();
  }

  std::ifstream ifs{locale_file};
  if (!ifs.is_open()) {
    logger::warn("Could not find locale config file %s", locale_file);
    return error_code::FILE_NOT_FOUND;
  }

  std::string line{};
  i32 equal_index = 0;
  i32 comment_index = 0;
  while (std::getline(ifs, line, '\n')) {
    equal_index = line.find('=');
    if (equal_index == -1) {
      continue;
    }

    comment_index = line.find('#');
    if (comment_index != -1 && comment_index < equal_index) {
      continue;
    }

    line[equal_index - 1] = '\0';
    texts.insert({parse_key(line.c_str()), line.substr(equal_index + 2)});
  }

  ifs.close();

  return error_code::OK;
}

// NOTE: Try to read this in the config file
const c8* locale::get_font() noexcept {
  switch (_locale) {
  case Locale::ENGLISH:
    return "assets/fonts/PixeloidSans.ttf";

  case Locale::JAPANESE:
    return "assets/fonts/jackeyfont.ttf";

  default:
    std::abort();
  }
}

i32 locale::get_size() noexcept {
  switch (_locale) {
  case Locale::ENGLISH:
    return 18;

  case Locale::JAPANESE:
    return 24;

  default:
    std::abort();
  }
}

const c8* locale::get_text(TextId id) noexcept {
  auto it = texts.find(id);
  return it == texts.end() ? nullptr : it->second.c_str();
}

} // namespace cfg

