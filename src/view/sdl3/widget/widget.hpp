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
  Widget() noexcept = default;
  Widget(const Widget&) noexcept = default;
  Widget& operator=(const Widget&) noexcept = default;
  Widget(Widget&&) noexcept = default;
  Widget& operator=(Widget&&) noexcept = default;
  virtual ~Widget() noexcept = default;

  /**
   * Tries to reset to original state, without any highlight
   **/
  virtual void reset() noexcept = 0;
  virtual void locale_updated(const Renderer& renderer) noexcept = 0;

  virtual void input(const event::Input& evt, Data& data) noexcept = 0;
  virtual void update() noexcept = 0;
  virtual void render(const Renderer& renderer) const noexcept = 0;

  // Position where to draw the widget
  frect rect{};
};

} // namespace view::sdl3

#endif

