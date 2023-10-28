/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#ifndef PXL_TOOL_PENCIL_HPP
#define PXL_TOOL_PENCIL_HPP

#include "./enum.hpp"
#include "model/model.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"

namespace tool {

class Pencil {
public:
  [[nodiscard]] u32 execute(Model& model, const event::Input& evt) noexcept;

private:
  /* i32 size = 1; */
  /* bool anti_aliasing = false; */
  /* bool pixel_perfect = false; */

  void handle_mouse_down(Model& model, input::MouseType type) noexcept;
  void handle_mouse_motion(Model& model) noexcept;
};

} // namespace tool

#endif

