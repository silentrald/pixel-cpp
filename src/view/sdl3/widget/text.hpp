/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#ifndef PXL_VIEW_SDL3_WIDGET_TEXT_HPP
#define PXL_VIEW_SDL3_WIDGET_TEXT_HPP

#include "../data.hpp"
#include "../renderer.hpp"
#include "../texture.hpp"
#include "./widget.hpp"
#include "core/cfg/locale.hpp"
#include "types.hpp"
#include "view/event.hpp"

namespace view::sdl3::widget {

class Text final : public Widget {
public:
  Text() noexcept = default;
  Text(const Text&) noexcept = delete;
  Text& operator=(const Text&) noexcept = delete;
  Text(Text&&) noexcept = default;
  Text& operator=(Text&&) noexcept = default;
  ~Text() noexcept override = default;

  void set_text(cfg::locale::TextId id, const Renderer& renderer) noexcept;

  void reset() noexcept override;
  void input(const event::Input& evt, Data& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

private:
  // Use the rect as the text_rect
  Texture tex{};
  cfg::locale::TextId id{};
  u32 info = 0; // Some state information about the text
};

} // namespace view::sdl3::widget

#endif

