/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-23
 *==========================*/

#ifndef PXL_VIEW_SDL3_DRAW_BOX_HPP
#define PXL_VIEW_SDL3_DRAW_BOX_HPP

#include "../input/button.hpp"
#include "../texture.hpp"
#include "./box.hpp"
#include "core/draw/anim.hpp"
#include "core/draw/types.hpp"
#include "core/ds/vector.hpp"
#include <array>

namespace view::sdl3::widget {

class DrawBox final : public Box {
public:
  DrawBox() noexcept = default;
  DrawBox(const DrawBox&) noexcept = delete;
  DrawBox& operator=(const DrawBox&) noexcept = delete;
  DrawBox(DrawBox&&) noexcept = default;
  DrawBox& operator=(DrawBox&&) noexcept = default;

  ~DrawBox() noexcept override = default;

  [[nodiscard]] error_code init_textures(ivec size) noexcept;

  [[nodiscard]] Texture& get_bg_texture() noexcept;
  [[nodiscard]] Texture& get_curr_texture() noexcept;
  [[nodiscard]] Texture& get_empty_texture() noexcept;
  [[nodiscard]] Texture& get_select1_texture() noexcept;
  [[nodiscard]] Texture& get_select2_texture() noexcept;

  [[nodiscard]] error_code set_anim(const draw::Anim* anim) noexcept;
  void set_active(usize frame_index, usize layer_index) noexcept;

  void update_curr_texture(usize img_id, bool show) noexcept;
  void update_bot_texture(usize frame_index, usize layer_index) noexcept;
  void update_top_texture(usize frame_index, usize layer_index) noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void update_locale() noexcept override;
  void input(const event::Input& evt, InputData& data) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

  frect draw_rect{50.0F, 50.0F, 320.0F, 320.0F};

private:
  // 0 - bg layer
  // 1 - bot layer
  // 2 - current layer
  // 3 - empty layer
  // 4 - top layer
  // 5 - select1 layer
  // 6 - select2 layer
  std::array<Texture, 7> textures{};
  const draw::Anim* anim = nullptr;
  ds::vector<draw::data_type> pixels{};
  i32 tick = 0;

  [[nodiscard]] Texture& get_bot_texture() noexcept;
  [[nodiscard]] Texture& get_top_texture() noexcept;
};

} // namespace view::sdl3::widget

#endif

