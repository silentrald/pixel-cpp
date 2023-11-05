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

  [[nodiscard]] const std::string& get_text() const noexcept;
  void set_text(const c8* str, const Renderer& renderer) noexcept;
  void push_text(const c8* str) noexcept;
  c8 pop_char() noexcept;
  void update_texture(const Renderer& renderer) noexcept;
  void reposition_text_rect(const Renderer& renderer) noexcept;

  void reset() noexcept override;
  void locale_updated(const Renderer& renderer) noexcept override;
  void input(const event::Input& evt, Data& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

private:
  std::string text{};
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

