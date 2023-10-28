/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#ifndef MODULES_TOOL_ERASER_HPP
#define MODULES_TOOL_ERASER_HPP

#include "./enum.hpp"
#include "model/model.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"

namespace tool {

class Eraser {
public:
  [[nodiscard]] u32 execute(Model& model, const event::Input& evt) noexcept;

private:
  /* i32 size = 1; */
  /* bool anti_aliasing = false; */
  /* bool pixel_perfect = false; */

  void handle_mouse_down(Model& model, fvec pos) noexcept;
  void handle_mouse_motion(Model& model, fvec pos) noexcept;
};

} // namespace tool

#endif

