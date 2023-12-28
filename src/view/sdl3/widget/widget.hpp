/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-22
 *==========================*/

#ifndef PXL_VIEW_SDL3_WIDGET_HPP
#define PXL_VIEW_SDL3_WIDGET_HPP

#include "../data.hpp"
#include "../renderer.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"

namespace view::sdl3 {

class Widget {
public:
  Widget() noexcept {}; // NOLINT
  Widget(const Widget&) noexcept = default;
  Widget& operator=(const Widget&) noexcept = default;
  Widget(Widget&&) noexcept = default;
  Widget& operator=(Widget&&) noexcept = default;
  virtual ~Widget() noexcept = default;

  /**
   * Tries to reset to original state, without any highlight
   **/
  virtual void reset() noexcept = 0;
  virtual void update_locale() noexcept = 0;

  virtual void input(const event::Input& evt, InputData& data) noexcept = 0;
  virtual void update(f32 delta) noexcept = 0;
  virtual void render() noexcept = 0;

  // Position where to draw the widget
  union {
    struct {
      f32 x = 0.0F;
      f32 y = 0.0F;
      f32 w = 0.0F;
      f32 h = 0.0F;
    };
    struct {
      fvec pos;
      fvec size;
    };
    frect rect;
  };
};

} // namespace view::sdl3

#endif

