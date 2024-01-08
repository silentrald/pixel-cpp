/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-22
 *==========================*/

#ifndef PXL_VIEW_SDL3_WIDGET_BTN_HPP
#define PXL_VIEW_SDL3_WIDGET_BTN_HPP

#include "../texture.hpp"
#include "./input.hpp"

namespace view::sdl3::widget {

class Button final : public Input {
public:
  Button() noexcept = default;
  Button(const Button&) noexcept = delete;
  Button& operator=(const Button&) noexcept = delete;
  Button(Button&&) noexcept = default;
  Button& operator=(Button&&) noexcept = default;
  ~Button() noexcept override = default;

  [[nodiscard]] bool is_enabled() const noexcept;

  void set_theme(input::BtnTheme theme) noexcept;
  void set_texture(Texture&& tex) noexcept;

  void enable() noexcept;
  void disable() noexcept;

  void reset() noexcept override;
  void unfocused() noexcept override;

  void update_locale() noexcept override;
  void input(const event::Input& evt) noexcept override;
  void key_input(event::KeyPress& keypress) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

private:
  Texture tex{};

public:
  // Rect container for the texture, Main rect will be used as the bg
  frect tex_rect{};

private:
  u32 info = input::BtnState::NORMAL;
};

} // namespace view::sdl3::widget

#endif

