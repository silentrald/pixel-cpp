/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#ifndef MODULES_TOOL_PAN_HPP
#define MODULES_TOOL_PAN_HPP

#include "model/model.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"

namespace tool {

class Pan {
public:
  [[nodiscard]] u32 execute(Model& model, const event::Input& evt) noexcept;

private:
  fvec origin{};
  fvec orig_pos{};

  void handle_mouse_down(Model& model, fvec pos) noexcept;
  void handle_mouse_motion(Model& model, fvec pos) const noexcept;
};

} // namespace tool

#endif

