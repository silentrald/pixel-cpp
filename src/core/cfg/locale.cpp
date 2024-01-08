/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#include "./locale.hpp"
#include "core/logger/logger.hpp"
#include <array>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>

namespace cfg {

namespace locale {

Locale _locale{};
std::ifstream ifs{}; // NOLINT
std::string line{};
std::array<u64, Section::SIZE> lookups{};

} // namespace locale

void locale::destroy_locale() noexcept {
  if (ifs.is_open()) {
    ifs.close();
  }
}

struct SectionHash {
  [[nodiscard]] u32 operator()(const c8* str) const noexcept {
    assert(str[0] != '\0');
    assert(str[1] != '\0');
    assert(str[2] != '\0');
    return *str;
  }
};

struct SectionCmp {
  [[nodiscard]] bool operator()(const c8* str1, const c8* str2) const noexcept {
    return std::strncmp(str1, str2, 4) == 0;
  }
};

// NOLINTNEXTLINE
#define SECTION_ENTRY(key)                                                     \
  { #key, locale::Section::key }
const std::unordered_map<const c8*, locale::Section, SectionHash, SectionCmp>
    // NOLINTNEXTLINE
    section_map{
        SECTION_ENTRY(GENERAL),         SECTION_ENTRY(MENU_ITEM),
        SECTION_ENTRY(FILE_CTX_MENU),   SECTION_ENTRY(EXPORT_CTX_MENU),
        SECTION_ENTRY(MODAL_TITLE),     SECTION_ENTRY(EDIT_CTX_MENU),
        SECTION_ENTRY(LAYERS_CTX_MENU), SECTION_ENTRY(TIMELINE_CTX_MENU),
    };
#undef SECTION_ENTRY

locale::Section parse_section(const char* str) noexcept {
  auto it = section_map.find(str);
  return it != section_map.end() ? it->second : locale::Section::SIZE;
}

error_code locale::load_locale(Locale locale) noexcept {
  _locale = locale;

  const c8* locale_file = nullptr;
  switch (locale) {
  case Locale::ENGLISH:
    locale_file = "locale/en.ini";
    break;

  case Locale::JAPANESE:
    locale_file = "locale/jp.ini";
    break;

  default:
    std::abort();
  }

  if (ifs.is_open()) {
    ifs.close();
  }

  locale::ifs.open(locale_file);
  if (!ifs.is_open()) {
    logger::warn("Could not find locale config file %s", locale_file);
    return error_code::FILE_NOT_FOUND;
  }

  u64 offset = 0U;
  while (std::getline(ifs, line, '\n')) {
    offset += line.size() + 1; // text + \n
    if (line[0] != '[') {
      continue;
    }

    line[line.size() - 1] = '\0';
    lookups[parse_section(line.c_str() + 1)] = offset;
  }
  ifs.clear();

  return error_code::OK;
}

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
  i32 equal_index = 0;
  i32 comment_index = 0;

  auto section = (0xffff'0000U & id) >> 16;
  id = (TextId)(0x0000'ffffU & id);
  ifs.seekg(lookups[section], std::ios::beg);

  while (std::getline(ifs, line, '\n')) {
    if (line[0] == '[') {
      // Would not find in the next section, just abort
      break;
    }

    equal_index = line.find('=');
    if (equal_index == -1) {
      continue;
    }

    comment_index = line.find('#');
    if (comment_index != -1 && comment_index < equal_index) {
      continue;
    }

    line[equal_index - 1] = '\0';
    if (id != (locale::TextId)locale::hash_locale(line.c_str())) {
      continue;
    }

    return line.c_str() + equal_index + 2;
  }

  logger::error("Could not find text id %04x'%04x", section, id);
  std::abort();
}

} // namespace cfg
