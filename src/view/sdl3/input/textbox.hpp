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
#include "./input.hpp"
#include <string>

namespace view::sdl3::widget {

class Textbox final : public Input {
public:
  Textbox() noexcept = default;
  Textbox(const Textbox&) noexcept = delete;
  Textbox& operator=(const Textbox&) noexcept = delete;
  Textbox(Textbox&&) noexcept = default;
  Textbox& operator=(Textbox&&) noexcept = default;
  ~Textbox() noexcept override = default;

  [[nodiscard]] const std::string& get_text() const noexcept;
  void set_text(const c8* str) noexcept;
  void push_text(const c8* str) noexcept;
  c8 pop_char() noexcept;
  void update_texture() noexcept;
  void reposition_text_rect() noexcept;

  void reset() noexcept override;
  void unfocused() noexcept override;

  void update_locale() noexcept override;
  void input(const event::Input& evt, InputData& data) noexcept override;
  void key_input(event::KeyPress& keypress) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

private:
  i32 cursor_blink_tick = 0;
  std::string text{};
  Texture tex{};

public:
  frect tex_rect{};
  void (*on_change)(const std::string& new_text) = nullptr;
};

} // namespace view::sdl3::widget

#endif

