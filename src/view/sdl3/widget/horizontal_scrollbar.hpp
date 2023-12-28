/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-17
 *==========================*/

#ifndef VIEW_SDL3_WIDGET_HORIZONTAL_SCROLLBAR_HPP
#define VIEW_SDL3_WIDGET_HORIZONTAL_SCROLLBAR_HPP

#include "./widget.hpp"
#include "types.hpp"

namespace view::sdl3::widget {

class HorizontalScrollbar final : public Widget {
public:
  void set_x(f32 x) noexcept;
  void set_y(f32 y) noexcept;
  void set_width(f32 width) noexcept;
  void set_height(f32 height) noexcept;
  void set_real_width(f32 real_width) noexcept;

  // Percentage is between 0.0F and 1.0F
  void set_percentage(f32 percentage) noexcept;

  // Between 0.0F and 1.0F
  [[nodiscard]] f32 get_percentage() const noexcept;

  void reset() noexcept override;
  void update_locale() noexcept override;

  void input(const event::Input& evt, InputData& data) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

private:
  frect scroll_rect{};
  f32 real_width = 0.0F;
  f32 max_offset = 0.0F;

  void adjust_widths() noexcept;
};

} // namespace view::sdl3::widget

#endif

