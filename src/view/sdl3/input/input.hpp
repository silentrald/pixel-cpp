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

  // For tabbing feature
  Input* next_input = nullptr;

private:
};

} // namespace view::sdl3::widget

#endif
