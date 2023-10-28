/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-25
 *==========================*/

#ifndef PXL_VIEW_SDL3_WINDOW_HPP
#define PXL_VIEW_SDL3_WINDOW_HPP

#include "SDL_video.h"
#include "core/logger/logger.hpp"
#include "types.hpp"

namespace view::sdl3 {

class Window {
public:
  Window() noexcept = default;
  Window(const Window&) noexcept = delete;
  Window& operator=(const Window&) noexcept = delete;
  Window(Window&&) noexcept = delete;
  Window& operator=(Window&&) noexcept = delete;

  inline void init() noexcept {
    // Create the window
    this->window = SDL_CreateWindow("Pixel", 1, 1, SDL_WINDOW_RESIZABLE);
    if (!this->window) {
      logger::fatal("Window could not be created");
      std::abort();
    }

    // Maximize and get the size of the window
    SDL_MaximizeWindow(this->window);
    SDL_GetWindowSize(this->window, &this->size.x, &this->size.y);
    SDL_SetWindowMinimumSize(this->window, 800, 600);
  }

  inline ~Window() noexcept {
    if (this->window) {
      SDL_DestroyWindow(this->window);
      this->window = nullptr;
    }
  }

  [[nodiscard]] inline SDL_Window* get_window() const noexcept {
    return this->window;
  }

  ivec size{};

private:
  SDL_Window* window = nullptr;
};

} // namespace view::sdl3

#endif

