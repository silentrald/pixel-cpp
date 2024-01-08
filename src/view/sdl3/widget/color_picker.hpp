/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-01
 *==========================*/

// NOTE: Implement popup if needed

#ifndef PXL_VIEW_WIDGET_COLOR_PICKER_HPP
#define PXL_VIEW_WIDGET_COLOR_PICKER_HPP

#include "../input/textbox.hpp"
#include "./widget.hpp"
#include "types.hpp"
#include <string>

namespace view::sdl3::widget {

class ColorPicker final : public Widget {
public:
  ColorPicker() noexcept = default;
  ColorPicker(const ColorPicker&) noexcept = delete;
  ColorPicker& operator=(const ColorPicker&) noexcept = delete;
  ColorPicker(ColorPicker&&) noexcept = default;
  ColorPicker& operator=(ColorPicker&&) noexcept = default;
  ~ColorPicker() noexcept override = default;

  void init(
      fvec pos, f32 size, void (*on_change)(const std::string& new_text)
  ) noexcept;
  void set_color(rgba8 color) noexcept;

  void reset() noexcept override;
  void update_locale() noexcept override;
  void input(const event::Input& evt) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

private:
  rgba8 color{};
  Textbox hex_str{};
};

} // namespace view::sdl3::widget

#endif

