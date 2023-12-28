/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-26
 *==========================*/

#ifndef PXL_VIEW_SDL3_STATUS_BOX_HPP
#define PXL_VIEW_SDL3_STATUS_BOX_HPP

#include "./box.hpp"

namespace view::sdl3::widget {

class StatusBox final : public Box {
public:
  StatusBox() noexcept = default;
  StatusBox(const StatusBox&) noexcept = delete;
  StatusBox& operator=(const StatusBox&) noexcept = delete;
  StatusBox(StatusBox&&) noexcept = default;
  StatusBox& operator=(StatusBox&&) noexcept = default;
  ~StatusBox() noexcept override = default;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void update_locale() noexcept override;
  void input(const event::Input& evt, InputData& data) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

  ivec pos{};
  ivec size{};

private:
};

} // namespace view::sdl3::widget

#endif

