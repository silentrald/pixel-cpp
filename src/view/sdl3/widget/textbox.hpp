/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#ifndef PXL_VIEW_SDL3_WIDGET_TEXTBOX_HPP
#define PXL_VIEW_SDL3_WIDGET_TEXTBOX_HPP

#include "../font.hpp"
#include "../texture.hpp"
#include "./widget.hpp"
#include <string>

namespace view::sdl3::widget {

class Textbox : public Widget {
public:
  Textbox() noexcept = default;
  Textbox(const Textbox&) noexcept = delete;
  Textbox& operator=(const Textbox&) noexcept = delete;
  Textbox(Textbox&&) noexcept = default;
  Textbox& operator=(Textbox&&) noexcept = default;
  ~Textbox() noexcept override = default;

  void update_texture(const Renderer& renderer, const Font& font) noexcept;

  void reset() noexcept override;
  void input(const event::Input& evt) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

  std::string text{};

private:
  Texture tex{};

public:
  frect tex_rect{};

private:
  i32 cursor_blink_tick = 0;

public:
  bool focused = false;
};

} // namespace view::sdl3::widget

#endif

