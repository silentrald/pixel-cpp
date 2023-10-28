/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-26
 *==========================*/

#include "./cached_textures.hpp"
#include "./renderer.hpp"
#include <cassert>

namespace view::sdl3 {

// TODO: Depends on locale, check utf-8 logic
void CachedTextures::init(const Font& font, const Renderer& renderer) noexcept {
  c8 str[2] = {' ', '\0'}; // NOLINT
  this->height = font.get_text_size(str).y;

  // Initialize the number textures
  for (i32 i = 0; i <= '~' - ' '; ++i) {
    str[0] = i + ' ';
    this->textures[i] = std::move(renderer.create_text(font, str));
    this->widths[i] = font.get_text_size(str).x;
  }
}

const Texture& CachedTextures::get_char_texture(c8 chr) const noexcept {
  assert(chr >= ' ' && chr <= '~');
  return this->textures[chr - ' '];
}

const Texture& CachedTextures::get_number_texture(i32 num) const noexcept {
  assert(num >= 0 && num <= 9);
  return this->textures[num - ' ' + '0'];
}

f32 CachedTextures::get_char_width(c8 chr) const noexcept {
  assert(chr >= ' ' && chr <= '~');
  return this->widths[chr - ' '];
}

[[nodiscard]] f32 CachedTextures::get_number_width(i32 num) const noexcept {
  assert(num >= 0 && num <= 9);
  return this->widths[num - ' ' + '0'];
}

[[nodiscard]] f32 CachedTextures::get_height() const noexcept {
  return this->height;
}

} // namespace view::sdl3
