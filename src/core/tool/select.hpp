/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-09-14
 *===============================*/

#ifndef MODULES_TOOL_SELECT_HPP
#define MODULES_TOOL_SELECT_HPP

#include "./enum.hpp"
#include "model/model.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"
#include <array>
#include <vector>

namespace tool {

enum class SelectState {
  RECT,
  // CIRCLE, WAND, LASSO
};

class Select {
public:
  [[nodiscard]] u32 execute(Model& model, const event::Input& evt) noexcept;

private:
  ivec origin{};
  // Data alloc bad
  std::array<std::vector<rgba8>, 2> pixels{};
  std::vector<bool> outline_mask{};
  ivec size{};
  SelectState state = SelectState::RECT;

  void handle_mouse_down(Model& model, fvec pos) noexcept;
  void handle_mouse_motion(Model& model, fvec pos) noexcept;
};

} // namespace tool

#endif

