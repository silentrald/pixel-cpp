/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-23
 *==========================*/

#ifndef PXL_VIEW_SDL3_DRAW_BOX_HPP
#define PXL_VIEW_SDL3_DRAW_BOX_HPP

#include "../texture.hpp"
#include "../widget/button.hpp"
#include "./box.hpp"
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

  void init_textures(const Renderer& renderer, ivec size) noexcept;
  [[nodiscard]] Texture& get_bg_texture() noexcept;
  [[nodiscard]] Texture& get_bot_texture() noexcept;
  [[nodiscard]] Texture& get_curr_texture() noexcept;
  [[nodiscard]] Texture& get_empty_texture() noexcept;
  [[nodiscard]] Texture& get_top_texture() noexcept;
  [[nodiscard]] Texture& get_select1_texture() noexcept;
  [[nodiscard]] Texture& get_select2_texture() noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void input(const event::Input& evt, Data& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

private:
  i32 tick = 0;

public:
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
};

} // namespace view::sdl3::widget

#endif

