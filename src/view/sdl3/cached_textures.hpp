/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-26
 *==========================*/

#ifndef PXL_VIEW_SDL3_CACHED_TEXTURES_HPP
#define PXL_VIEW_SDL3_CACHED_TEXTURES_HPP

#include "./font.hpp"
#include "./texture.hpp"
#include "types.hpp"
#include <array>

namespace view::sdl3 {

class Renderer;

class CachedTextures {
public:
  CachedTextures() noexcept = default;
  CachedTextures(const CachedTextures&) noexcept = delete;
  CachedTextures& operator=(const CachedTextures&) noexcept = delete;
  CachedTextures(CachedTextures&& rhs) noexcept = delete;
  CachedTextures& operator=(CachedTextures&& rhs) noexcept = delete;
  ~CachedTextures() noexcept = default;

  void init(const Renderer& renderer) noexcept;
  void locale_updated(const Renderer& renderer) noexcept;

  [[nodiscard]] const Texture& get_char_texture(c8 chr) const noexcept;
  [[nodiscard]] const Texture& get_number_texture(i32 num) const noexcept;
  [[nodiscard]] f32 get_char_width(c8 chr) const noexcept;
  [[nodiscard]] f32 get_number_width(i32 num) const noexcept;
  [[nodiscard]] f32 get_height() const noexcept;

private:
  // NOTE: Should be map instead of array to support multiple locales
  std::array<Texture, '~' - ' ' + 1U> textures{};
  std::array<f32, '~' - ' ' + 1U> widths{};
  f32 height = 0.0F;
};

} // namespace view::sdl3

#endif

