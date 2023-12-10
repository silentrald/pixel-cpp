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

namespace view::sdl3::renderer {

void init(SDL_Window* window) noexcept;
void destroy() noexcept;

void locale_updated() noexcept;

// Clears the screen
void clear() noexcept;

// Presents the screen
void present() noexcept;

// === Draw === //

void set_color(rgba8 color) noexcept;
void fill_rect(const frect& rect) noexcept;
void draw_rect(const frect& rect) noexcept;

// NOTE: Only supports rgba8 for now
[[nodiscard]] Texture create_texture(ivec size) noexcept;

[[nodiscard]] fvec get_text_size(const c8* str) noexcept;
[[nodiscard]] f32 get_text_height() noexcept;
[[nodiscard]] Texture create_text(const c8* str) noexcept;

[[nodiscard]] Texture load_img(const c8* path) noexcept;

void render_texture(const Texture& texture, const frect& rect) noexcept;

// === Fast Renders === //

/**
 * Renders a number on the renderer:: Only use this for dynamic numbers being
 * rendered on the screen.
 *
 * @param num - number to be rendered
 * @param pos - anchored at the top right point
 **/
[[nodiscard]] frect render_number(i32 num, fvec pos) noexcept;

/**
 * Renders a text on the renderer:: Only use this for dynamic texts being
 * rendered on the screen.
 *
 * @param num - number to be rendered
 * @param pos - anchored at the top left point
 **/
void render_text(const c8* str, fvec pos) noexcept;

} // namespace view::sdl3::renderer

#endif

