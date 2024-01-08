/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-25
 *==========================*/

#ifndef PXL_VIEW_SDL3_MENU_BOX_HPP
#define PXL_VIEW_SDL3_MENU_BOX_HPP

#include "../widget/text.hpp"
#include "./box.hpp"
#include "core/cfg/locale.hpp"
#include "core/ds/vector.hpp"

namespace view::sdl3::widget {

class MenuBox final : public Box {
public:
  MenuBox() noexcept = default;
  MenuBox(const MenuBox&) noexcept = delete;
  MenuBox& operator=(const MenuBox&) noexcept = delete;
  MenuBox(MenuBox&&) noexcept = default;
  ~MenuBox() noexcept override = default;
  MenuBox& operator=(MenuBox&&) noexcept = default;

  [[nodiscard]] error_code
  push_menu_btn(cfg::locale::TextId text_id, void (*listener)()) noexcept;

  [[nodiscard]] const frect& get_btn_rect(usize index) const noexcept;
  [[nodiscard]] usize get_btns_size() const noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void update_locale() noexcept override;
  void input(const event::Input& evt) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

private:
  struct MenuBtn {
    frect rect{};
    Text text{};
    void (*listener)() = nullptr;
    input::BtnState state = input::BtnState::NORMAL;
  };

  ds::vector<MenuBtn> btns{};

public:
  i32 selected = -1;
};

} // namespace view::sdl3::widget

#endif

