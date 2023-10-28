/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-09
 *==========================*/

#ifndef PXL_VIEW_SDL3_TEXTURE_HPP
#define PXL_VIEW_SDL3_TEXTURE_HPP

#include "SDL_render.h"
#include "types.hpp"
#include <cassert>

namespace view::sdl3 {

template <typename Color> class Pixels;

/**
 * Image data for rendering on the screen
 **/
class Texture {
public:
  Texture() noexcept = default;
  explicit Texture(SDL_Texture* tex) noexcept;
  Texture(const Texture&) noexcept = delete;
  Texture& operator=(const Texture&) noexcept = delete;
  Texture(Texture&& rhs) noexcept;
  Texture& operator=(Texture&& rhs) noexcept;
  ~Texture() noexcept;

  void set_texture(SDL_Texture* tex) noexcept;

  [[nodiscard]] SDL_Texture* get_texture() const noexcept;
  [[nodiscard]] ivec get_size() const noexcept;

  void clear(i32 height) noexcept;

  template <typename Color> void set_pixels(Color* pixels, ivec size) noexcept;

  template <typename Color>
  [[nodiscard]] Pixels<Color> lock_texture() noexcept {
    Color* ptr = nullptr;
    i32 pitch = 0;
    SDL_LockTexture(this->tex, nullptr, (void**)&ptr, &pitch);
    return Pixels<Color>{ptr, this->tex};
  }

  template <typename Color> void paint(ivec pos, Color color) noexcept {
    assert(
        pos.x >= 0 && pos.x < this->get_size().x && pos.y >= 0 &&
        pos.y < this->get_size().y
    );
    Color* ptr = nullptr;
    i32 pitch = 0;
    SDL_LockTexture(this->tex, nullptr, (void**)&ptr, &pitch);
    ptr[pos.x + pos.y * this->get_size().x] = color;
    SDL_UnlockTexture(this->tex);
  }

  template <typename Color> void paint(i32 index, Color color) noexcept {
    assert(index >= 0 && index < this->get_size().x * this->get_size().y);
    Color* ptr = nullptr;
    i32 pitch = 0;
    SDL_LockTexture(this->tex, nullptr, (void**)&ptr, &pitch);
    ptr[index] = color;
    SDL_UnlockTexture(this->tex);
  }

private:
  SDL_Texture* tex = nullptr;
};

/**
 * Pixels from textures, will lock the texture until this object is freed
 **/
template <typename Color> class Pixels {
public:
  explicit Pixels(Color* ptr, SDL_Texture* texture) noexcept
      : ptr(ptr), texture(texture) {}

  Pixels(const Pixels&) noexcept = delete;
  Pixels& operator=(const Pixels&) noexcept = delete;

  Pixels(Pixels&& rhs) noexcept : ptr(rhs.ptr), texture(rhs.texture) {
    rhs.ptr = nullptr;
    rhs.texture = nullptr;
  };

  Pixels& operator=(Pixels&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->ptr = rhs.ptr;
    this->texture = rhs.texture;

    rhs.ptr = nullptr;
    rhs.texture = nullptr;

    return *this;
  }

  ~Pixels() noexcept {
    if (this->texture) {
      SDL_UnlockTexture(this->texture);
      this->texture = nullptr;
    }
  }

  Color* get_ptr() noexcept {
    return this->ptr;
  }

  void inline paint(i32 index, Color color) noexcept {
    this->ptr[index] = color;
  }

private:
  Color* ptr = nullptr;
  SDL_Texture* texture = nullptr;
};

} // namespace view::sdl3

#endif

