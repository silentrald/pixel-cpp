/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-29
 *==========================*/

#ifndef PXL_VIEW_SDL3_INPUT_HPP
#define PXL_VIEW_SDL3_INPUT_HPP

#include "../widget/widget.hpp"

namespace view::sdl3::widget {

class Input : public virtual Widget { // NOLINT
public:
  Input() noexcept = default;
  Input(const Input&) noexcept = delete;
  Input& operator=(const Input&) noexcept = delete;
  Input(Input&&) noexcept = default;
  Input& operator=(Input&&) noexcept = default;

  virtual void unfocused(const Renderer& renderer) noexcept = 0;

  virtual void key_input(
      const event::KeyPress& keypress, const Renderer& renderer
  ) noexcept = 0;

  // For tabbing feature
  Input* next_input = nullptr;
  bool focused = false;
};

} // namespace view::sdl3::widget

#endif

