/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-04
 *==========================*/

#ifndef PXL_DRAW_FRAME_HPP
#define PXL_DRAW_FRAME_HPP

#include "./types.hpp"
#include "types.hpp"

namespace draw {

class Frame {
public:
  Frame() noexcept = default;
  Frame(usize* ptr, usize layer_count, usize id) noexcept;
  Frame(const Frame&) noexcept = default;
  Frame& operator=(const Frame&) noexcept = default;
  Frame(Frame&&) noexcept = default;
  Frame& operator=(Frame&&) noexcept = default;
  ~Frame() noexcept = default;

  [[nodiscard]] u32 get_id() const noexcept;
  [[nodiscard]] u32 get_image_id(usize index) const noexcept;
  [[nodiscard]] i32 get_layer_count() const noexcept;

private:
  u32* ptr = nullptr;
  i32 layer_count = 0;
  u32 id = 0U;
};

} // namespace draw

#endif

