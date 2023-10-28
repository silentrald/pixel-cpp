/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-08
 *==========================*/

#ifndef PXL_VIEW_SDL3_RENDERER_HPP
#define PXL_VIEW_SDL3_RENDERER_HPP

#include "./cached_textures.hpp"
#include "./font.hpp"
#include "./texture.hpp"
#include "SDL_render.h"

namespace view::sdl3 {

class Renderer {
public:
  Renderer() noexcept = default;
  Renderer(const Renderer&) noexcept = delete;
  Renderer& operator=(const Renderer&) noexcept = delete;
  Renderer(Renderer&& rhs) noexcept;
  Renderer& operator=(Renderer&& rhs) noexcept;
  ~Renderer() noexcept;

  void init(SDL_Window* window, const Font& font) noexcept;

  // Clears the screen
  void clear() noexcept;
  // Presents the screen
  void present() noexcept;

  // === Draw === //

  void set_color(rgba8 color) const noexcept;
  void fill_rect(const frect& rect) const noexcept;
  void draw_rect(const frect& rect) const noexcept;

  // NOTE: Only supports rgba8 for now
  [[nodiscard]] Texture create_texture(ivec size) const noexcept;
  [[nodiscard]] Texture
  create_text(const Font& font, const c8* str) const noexcept;
  [[nodiscard]] Texture load_img(const c8* path) const noexcept;

  void render_texture(const Texture& texture, const frect& rect) const noexcept;

  // === Fast Renders === //

  /**
   * Renders a number on the renderer. Only use this for dynamic numbers being
   * rendered on the screen.
   *
   * @param num - number to be rendered
   * @param pos - anchored at the top right point
   **/
  [[nodiscard]] frect render_number(i32 num, fvec pos) const noexcept;

  /**
   * Renders a text on the renderer. Only use this for dynamic texts being
   * rendered on the screen.
   *
   * @param num - number to be rendered
   * @param pos - anchored at the top left point
   **/
  void render_text(const c8* str, fvec pos) const noexcept;

private:
  SDL_Renderer* renderer = nullptr;
  CachedTextures textures{};
};

} // namespace view::sdl3

#endif

