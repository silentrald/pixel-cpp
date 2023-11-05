/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-26
 *==========================*/

#ifndef PXL_VIEW_SDL3_FONT_HPP
#define PXL_VIEW_SDL3_FONT_HPP

#include "SDL3_ttf/SDL_ttf.h"
#include "core/logger/logger.hpp"
#include "types.hpp"
#include <cassert>
#include <cstdlib>

namespace view::sdl3 {

class Font {
public:
  Font() noexcept = default;
  Font(const Font&) noexcept = delete;
  Font& operator=(const Font&) noexcept = delete;

  inline Font(Font&& rhs) noexcept : font(rhs.font) {
    rhs.font = nullptr;
  }

  inline Font& operator=(Font&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->font = rhs.font;
    rhs.font = nullptr;

    return *this;
  }

  // TODO: std::expected
  void init(const c8* path, i32 size) noexcept {
    this->font = TTF_OpenFont(path, size);
    if (this->font == nullptr) {
      logger::fatal("Could not load font");
      std::abort();
    }
  }

  void set(const c8* path, i32 size) noexcept {
    if (this->font) {
      TTF_CloseFont(this->font);
    }

    this->font = TTF_OpenFont(path, size);
    if (this->font == nullptr) {
      logger::fatal("Could not load font");
      std::abort();
    }
  }

  ~Font() noexcept {
    if (this->font) {
      TTF_CloseFont(this->font);
      this->font = nullptr;
    }
  }

  [[nodiscard]] inline TTF_Font* get_font() const noexcept {
    assert(this->font != nullptr);
    return this->font;
  }

  [[nodiscard]] inline fvec get_text_size(const c8* str) const noexcept {
    assert(this->font != nullptr && str != nullptr);
    ivec size{};
    TTF_SizeUTF8(this->font, str, &size.x, &size.y);
    return {.x = (f32)size.x, .y = (f32)size.y};
  }

private:
  TTF_Font* font = nullptr;
};

} // namespace view::sdl3

#endif

