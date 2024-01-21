/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-05
 *==========================*/

#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL_init.h"
#include "core/logger/logger.hpp"
#include "presenter/presenter.hpp"

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    logger::fatal("Could not initialize sdl3");
    return -1;
  }

  if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
    logger::fatal("Could not initialize sdl3 image");
    return -1;
  }

  if (TTF_Init() != 0) {
    logger::fatal("Could not initialize sdl3 fonts");
    return -1;
  }

  SDL_StopTextInput();

  {
    presenter::init();
    presenter::run();
  }

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

  return 0;
}
