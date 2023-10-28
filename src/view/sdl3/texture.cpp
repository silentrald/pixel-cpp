/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-09
 *==========================*/

#include "./texture.hpp"
#include <cstring>

namespace view::sdl3 {

Texture::Texture(SDL_Texture* tex) noexcept : tex(tex) {}

void Texture::set_texture(SDL_Texture* tex) noexcept {
  if (this->tex) {
    SDL_DestroyTexture(this->tex);
  }
  this->tex = tex;
}

Texture::Texture(Texture&& rhs) noexcept : tex(rhs.tex) {
  rhs.tex = nullptr;
}

Texture& Texture::operator=(Texture&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->tex = rhs.tex;
  rhs.tex = nullptr;

  return *this;
}

Texture::~Texture() noexcept {
  if (this->tex) {
    SDL_DestroyTexture(this->tex);
    this->tex = nullptr;
  }
}

SDL_Texture* Texture::get_texture() const noexcept {
  return this->tex;
}

ivec Texture::get_size() const noexcept {
  ivec size{};
  SDL_QueryTexture(this->tex, nullptr, nullptr, &size.x, &size.y);
  return size;
}

void Texture::clear(i32 height) noexcept {
  rgba8* curr_pixels = nullptr;
  i32 pitch = 0;
  SDL_LockTexture(this->tex, nullptr, (void**)&curr_pixels, &pitch);
  // NOLINTNEXTLINE
  std::memset(curr_pixels, 0, pitch * height);
  SDL_UnlockTexture(this->tex);
}

template <> void Texture::set_pixels(rgba8* pixels, ivec size) noexcept {
  rgba8* curr_pixels = nullptr;
  i32 pitch = 0;
  SDL_LockTexture(this->tex, nullptr, (void**)&curr_pixels, &pitch);
  // NOLINTNEXTLINE
  std::memcpy(curr_pixels, pixels, size.x * size.y * sizeof(rgba8));
  SDL_UnlockTexture(this->tex);
}

} // namespace view::sdl3

