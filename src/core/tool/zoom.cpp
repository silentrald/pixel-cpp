/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#include "./zoom.hpp"
#include <algorithm>

namespace tool {

u32 Zoom::execute(Model& model, const event::Input& evt) noexcept {
  if (evt.mouse.wheel.y == 0) {
    return event::Flag::NONE;
  }

  f32 new_scale = std::clamp(model.scale + evt.mouse.wheel.y, 1.0F, 32.0F);
  if (new_scale == model.scale) {
    return event::Flag::NONE;
  }

  // Compute for the new rect value here
  model.rect.w = model.anim.get_width() * new_scale;
  model.rect.h = model.anim.get_height() * new_scale;

  // Matrix multiply evaluated (translate, scale, translate back)
  f32 ratio = new_scale / model.scale;
  model.rect.x = (model.rect.x - evt.mouse.pos.x) * ratio + evt.mouse.pos.x;
  model.rect.y = (model.rect.y - evt.mouse.pos.y) * ratio + evt.mouse.pos.y;

  model.scale = new_scale;

  // Clamping
  model.rect.x = std::clamp(
      model.rect.x, model.bounds.x - model.rect.w + 32.0F,
      model.bounds.x + model.bounds.w - 32.0F
  );
  model.rect.y = std::clamp(
      model.rect.y, model.bounds.y - model.rect.h + 32.0F,
      model.bounds.y + model.bounds.h - 32.0F
  );

  return event::Flag::NONE;
}

} // namespace tool

