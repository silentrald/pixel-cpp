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

void Renderer::init(SDL_Window* window) noexcept {
  this->renderer =
      SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
  if (!this->renderer) {
    logger::fatal("Renderer could not be created");
    std::abort();
  }

  this->font.init(cfg::locale::get_font(), cfg::locale::get_size());

  // For transparency of textures
  SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);

  this->textures.init(*this);
}

void Renderer::locale_updated() noexcept {
  this->font.set(cfg::locale::get_font(), cfg::locale::get_size());
  this->textures.locale_updated(*this);
}

Renderer::~Renderer() noexcept {
  if (this->renderer) {
    SDL_DestroyRenderer(this->renderer);
    this->renderer = nullptr;
  }
}

Renderer::Renderer(Renderer&& rhs) noexcept : renderer(rhs.renderer) {
  rhs.renderer = nullptr;
}

Renderer& Renderer::operator=(Renderer&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->renderer = rhs.renderer;
  rhs.renderer = nullptr;

  return *this;
}

void Renderer::clear() noexcept {
  SDL_RenderClear(this->renderer);
}

void Renderer::present() noexcept {
  SDL_RenderPresent(this->renderer);
}

void Renderer::set_color(rgba8 color) const noexcept {
  SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
}

void Renderer::fill_rect(const frect& rect) const noexcept {
  SDL_RenderFillRect(this->renderer, (SDL_FRect*)&rect);
}

Texture Renderer::create_texture(ivec size) const noexcept {
  SDL_Texture* tex = SDL_CreateTexture(
      this->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
      size.x, size.y
  );

  if (tex) {
    // For transparency
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  }

  return Texture{tex};
}

fvec Renderer::get_text_size(const c8* str) const noexcept {
  return this->font.get_text_size(str);
}

f32 Renderer::get_text_height() const noexcept {
  return this->textures.get_height();
}

Texture Renderer::create_text(const c8* str) const noexcept {
  assert(str != nullptr);

  if (str[0] == '\0') {
    return Texture{};
  }

  SDL_Surface* surface = TTF_RenderUTF8_Solid(
      this->font.get_font(), str, {0x00U, 0x00U, 0x00U, 0xffU}
  );
  if (surface == nullptr) {
    logger::error("Could not create text texture: surface error");
    return Texture{};
  }

  // tex can be nullptr
  SDL_Texture* tex = SDL_CreateTextureFromSurface(this->renderer, surface);
  SDL_DestroySurface(surface);
  if (tex == nullptr) {
    logger::error("Could not create text texture: texture error");
    return Texture{};
  }

  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  return Texture{tex};
}

Texture Renderer::load_img(const c8* path) const noexcept {
  assert(path != nullptr);

  SDL_Texture* tex = IMG_LoadTexture(this->renderer, path);
  if (tex == nullptr) {
    logger::error("Could not load image %s", path);
  }

  return Texture{tex};
}

void Renderer::render_texture(const Texture& texture, const frect& rect)
    const noexcept {
  SDL_RenderTexture(
      this->renderer, texture.get_texture(), nullptr, (SDL_FRect*)&rect
  );
}

void Renderer::draw_rect(const frect& rect) const noexcept {
  SDL_RenderRect(this->renderer, (SDL_FRect*)&rect);
}

frect Renderer::render_number(i32 num, fvec pos) const noexcept {
  frect rect{.pos = pos, .size = {.y = this->textures.get_height()}};

  if (num == 0) {
    // only draw the 0
    rect.w = this->textures.get_number_width(0);
    rect.x -= rect.w;
    this->render_texture(this->textures.get_number_texture(0), rect);
    return rect;
  }

  i32 tmp = 0;
  f32 total_width = 0.0F;
  while (num > 0) {
    tmp = num % 10;
    num /= 10;

    rect.w = this->textures.get_number_width(tmp);
    rect.x -= rect.w;
    total_width += rect.w;
    this->render_texture(this->textures.get_number_texture(tmp), rect);
  }

  return rect;
}

void Renderer::render_text(const c8* str, fvec pos) const noexcept {
  assert(str != nullptr);

  frect rect{.pos = pos, .size = {.y = this->textures.get_height()}};

  for (i32 i = 0; str[i] != '\0'; ++i) {
    rect.w = this->textures.get_char_width(str[i]);
    this->render_texture(this->textures.get_char_texture(str[i]), rect);
    rect.x += rect.w;
  }
}

} // namespace view::sdl3
