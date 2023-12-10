/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-09
 *==========================*/

#ifndef PXL_MODEL_HPP
#define PXL_MODEL_HPP

#include "core/cfg/locale.hpp"
#include "core/draw/anim.hpp"
#include "core/draw/image.hpp"
#include "core/ds/vector.hpp"
#include "core/tool/enum.hpp"
#include "types.hpp"
#include "view/core.hpp"
#include <vector>

struct Model {
  draw::Anim anim{};
  draw::Image img{};
  Texture* tex1 = nullptr;
  Texture* tex2 = nullptr;
  frect rect{};
  frect bounds{};
  f32 scale = 10.0F;

  // Temp data
  ivec curr_pos{};
  ivec prev_pos{};
  rgba8 color{};

  // Cache
  ds::vector<draw::data_type> pixels{};
  ds::vector<draw::data_type> orig_pixels{}; // used by edit image action

  // State
  bool is_editing_image = false;

  //
  cfg::locale::Locale locale = cfg::locale::Locale::ENGLISH;
  tool::Type tool = tool::Type::PENCIL;
  usize frame_index = 0U;
  usize layer_index = 0U;
  usize selected_frame = 0U;
  usize selected_layer = 0U;
  usize img_id = 1U;
  rgba8 fg_color{0x00, 0x00, 0x00, 0xff};
  rgba8 bg_color{0xff, 0xff, 0xff, 0xff};
  // NOTE: ds::vector<bool> is not optimized like std::vector<bool>
  std::vector<bool> select_mask{};

  [[nodiscard]] i32 get_pixel_index() const noexcept {
    return this->curr_pos.x + this->curr_pos.y * this->anim.get_width();
  }
};

#endif

