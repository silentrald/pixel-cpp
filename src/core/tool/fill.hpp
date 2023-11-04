/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-29
 *===============================*/

#ifndef PXL_TOOL_FILL_HPP
#define PXL_TOOL_FILL_HPP

#include "../draw/image.hpp"
#include "./enum.hpp"
#include "model/model.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"
#include <vector>

namespace tool {

/**
 * Refer: https://en.wikipedia.org/wiki/Flood_fill
 **/
class Fill {
public:
  [[nodiscard]] u32 execute(Model& model, const event::Input& evt) noexcept;

private:
  rgba8 new_color{};
  rgba8 old_color{};

  [[nodiscard]] inline bool check_pixel(
      const std::vector<bool>& mask, const draw::Image& image, ivec pos,
      i32 width
  ) const noexcept;

  /**
   * Refer: https://lodev.org/cgtutor/floodfill.html
   **/
  void scan_fill(
      draw::Image& image, Texture& texture, ivec size, ivec pos,
      const std::vector<bool>& mask
  ) noexcept;
};

} // namespace tool

#endif

