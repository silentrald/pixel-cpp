/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-24
 *==========================*/

#ifndef PXL_VIEW_SDL3_CONTEXT_MENU_HPP
#define PXL_VIEW_SDL3_CONTEXT_MENU_HPP

#include "../widget/text.hpp"
#include "../widget/widget.hpp"
#include "core/cfg/locale.hpp"
#include "core/ds/vector.hpp"

namespace view::sdl3::widget {

class ContextMenu final : public Widget {
public:
  ContextMenu() noexcept = default;
  ContextMenu(const ContextMenu&) noexcept = delete;
  ContextMenu& operator=(const ContextMenu&) noexcept = delete;
  ContextMenu(ContextMenu&&) noexcept = default;
  ContextMenu& operator=(ContextMenu&&) noexcept = default;
  ~ContextMenu() noexcept override = default;

  [[nodiscard]] error_code
  push_item(cfg::locale::TextId text_id, void (*listener)()) noexcept;
  void set_disabled(usize index, bool disabled) noexcept;

  [[nodiscard]] fvec get_sel_item_pos() const noexcept;

  void reposition(fvec pos) noexcept;

  void reset() noexcept override;
  void update_locale() noexcept override;

  void input(const event::Input& evt) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

private:
  struct Item {
    Text text{};
    frect rect{};
    input::BtnState state{};
    void (*listener)() = nullptr;
  };

  ds::vector<Item> items{};
  usize selected = 0U;
};

} // namespace view::sdl3::widget

#endif

