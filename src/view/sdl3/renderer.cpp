/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-08
 *==========================*/

#include "./renderer.hpp"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL_blendmode.h"
#include "SDL_oldnames.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include "core/cfg/locale.hpp"
#include "core/logger/logger.hpp"
#include <cstdio>
#include <cstdlib>

namespace view::sdl3 {

SDL_Renderer* renderer_ = nullptr;
Font font_{};
CachedTextures textures_{};

// === renderer Implementation === //

void renderer::init(SDL_Window* window) noexcept {
  renderer_ = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
  if (renderer_ == nullptr) {
    logger::fatal("renderer could not be created");
    std::abort();
  }

  font_.init(cfg::locale::get_font(), cfg::locale::get_size());

  // For transparency of textures
  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

  textures_.init();
}

void renderer::destroy() noexcept {
  font_.~Font();
  textures_.~CachedTextures();

  if (renderer_) {
    SDL_DestroyRenderer(renderer_);
    renderer_ = nullptr;
  }
}

void renderer::update_locale() noexcept {
  font_.set(cfg::locale::get_font(), cfg::locale::get_size());
  textures_.update_locale();
}

void renderer::clear() noexcept {
  SDL_RenderClear(renderer_);
}

void renderer::present() noexcept {
  SDL_RenderPresent(renderer_);
}

void renderer::set_color(rgba8 color) noexcept {
  SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
}

void renderer::fill_rect(const frect& rect) noexcept {
  SDL_RenderFillRect(renderer_, (SDL_FRect*)&rect);
}

Texture renderer::create_texture(ivec size) noexcept {
  SDL_Texture* tex = SDL_CreateTexture(
      renderer_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, size.x,
      size.y
  );

  if (tex) {
    // For transparency
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  }

  return Texture{tex};
}

fvec renderer::get_text_size(const c8* str) noexcept {
  return font_.get_text_size(str);
}

f32 renderer::get_text_height() noexcept {
  return textures_.get_height();
}

Texture renderer::create_text(const c8* str) noexcept {
  assert(str != nullptr);

  if (str[0] == '\0') {
    return Texture{};
  }

  SDL_Surface* surface =
      TTF_RenderUTF8_Solid(font_.get_font(), str, {0x00U, 0x00U, 0x00U, 0xffU});
  if (surface == nullptr) {
    logger::error("Could not create text texture: surface error");
    return Texture{};
  }

  // tex can be nullptr
  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surface);
  SDL_DestroySurface(surface);
  if (tex == nullptr) {
    logger::error("Could not create text texture: texture error");
    return Texture{};
  }

  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  return Texture{tex};
}

Texture renderer::load_img(const c8* path) noexcept {
  assert(path != nullptr);

  SDL_Texture* tex = IMG_LoadTexture(renderer_, path);
  if (tex == nullptr) {
    logger::error("Could not load image %s", path);
  }

  return Texture{tex};
}

void renderer::render_texture(
    const Texture& texture, const frect& rect
) noexcept {
  SDL_RenderTexture(
      renderer_, texture.get_texture(), nullptr, (SDL_FRect*)&rect
  );
}

void renderer::draw_rect(const frect& rect) noexcept {
  SDL_RenderRect(renderer_, (SDL_FRect*)&rect);
}

frect renderer::render_number(i32 num, fvec pos) noexcept {
  frect rect{.pos = pos, .size = {.y = textures_.get_height()}};

  if (num == 0) {
    // only draw the 0
    rect.w = textures_.get_number_width(0);
    rect.x -= rect.w;
    renderer::render_texture(textures_.get_number_texture(0), rect);
    return rect;
  }

  i32 tmp = 0;
  f32 total_width = 0.0F;
  while (num > 0) {
    tmp = num % 10;
    num /= 10;

    rect.w = textures_.get_number_width(tmp);
    rect.x -= rect.w;
    total_width += rect.w;
    render_texture(textures_.get_number_texture(tmp), rect);
  }

  return rect;
}

void renderer::render_text(const c8* str, fvec pos) noexcept {
  assert(str != nullptr);

  frect rect{.pos = pos, .size = {.y = textures_.get_height()}};

  for (i32 i = 0; str[i] != '\0'; ++i) {
    rect.w = textures_.get_char_width(str[i]);
    render_texture(textures_.get_char_texture(str[i]), rect);
    rect.x += rect.w;
  }
}

} // namespace view::sdl3

