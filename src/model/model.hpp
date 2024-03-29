/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-09
 *==========================*/

#ifndef PXL_MODEL_HPP
#define PXL_MODEL_HPP

#include "core/draw/anim.hpp"
#include "core/draw/layer.hpp"
#include "core/tool/enum.hpp"
#include "types.hpp"
#include <vector>

// TODO: Depends on the frontend renderer
#include "view/sdl3/texture.hpp"
using Texture = view::sdl3::Texture;

struct Model {
  draw::Anim anim{};
  draw::Layer layer{};
  Texture* tex1 = nullptr;
  Texture* tex2 = nullptr;
  frect rect{};
  frect bounds{};
  f32 scale = 10.0F;

  // Temp data
  ivec curr_pos{};
  ivec prev_pos{};
  rgba8 color{};

  //
  tool::Type tool = tool::Type::PENCIL;
  i32 frame_index = 0;
  i32 layer_index = 0;
  rgba8 fg_color{0x00, 0x00, 0x00, 0xff};
  rgba8 bg_color{0xff, 0xff, 0xff, 0xff};
  std::vector<bool> select_mask{};

  [[nodiscard]] i32 get_pixel_index() const noexcept {
    return this->curr_pos.x + this->curr_pos.y * this->anim.get_width();
  }
};

#endif

