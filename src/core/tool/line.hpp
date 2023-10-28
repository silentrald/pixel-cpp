/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#ifndef PXL_TOOL_LINE_HPP
#define PXL_TOOL_LINE_HPP

#include "./enum.hpp"
#include "model/model.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"

namespace tool {

class Line {
public:
  [[nodiscard]] u32 execute(Model& model, const event::Input& evt) noexcept;

private:
  ivec origin{};
  fvec center{};

  void
  handle_mouse_down(Model& model, input::MouseType type, fvec pos) noexcept;

  /**
   * Get a point that snaps thru a perfect line
   **/
  [[nodiscard]] ivec
  get_snapping_point(fvec pos, fvec offset, i32 scale) const noexcept;

  void handle_mouse_motion(Model& model, input::KeyMod key, fvec pos) noexcept;
  void handle_mouse_up(Model& model, input::KeyMod key, fvec pos) noexcept;
};

} // namespace tool

#endif

