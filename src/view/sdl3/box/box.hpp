/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#ifndef PXL_VIEW_SDL3_BOX_HPP
#define PXL_VIEW_SDL3_BOX_HPP

#include "../widget/widget.hpp"

namespace view::sdl3::widget {

/**
 * Widget object that contains widgets within it
 **/
class Box : public virtual Widget { // NOLINT
public:
  Box() noexcept = default;
  Box(const Box&) noexcept = delete;
  Box& operator=(const Box&) noexcept = delete;
  Box(Box&&) noexcept = default;
  Box& operator=(Box&&) noexcept = default;

  /**
   * Call for responsive elements within the box
   **/
  virtual void resize(const frect& rect) noexcept = 0;
};

} // namespace view::sdl3::widget

#endif

