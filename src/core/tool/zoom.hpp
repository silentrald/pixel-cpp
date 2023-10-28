/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#ifndef MODULES_TOOL_ZOOM_HPP
#define MODULES_TOOL_ZOOM_HPP

#include "model/model.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"

namespace tool {

class Zoom {
public:
  [[nodiscard]] u32 execute(Model& model, const event::Input& evt) noexcept;

private:
};

} // namespace tool

#endif

