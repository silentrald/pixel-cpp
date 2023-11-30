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

  [[nodiscard]] error_code push_menu_btn(
      cfg::locale::TextId text_id, const Renderer& renderer, void (*listener)()
  ) noexcept;

  [[nodiscard]] const frect& get_btn_rect(usize index) const noexcept;
  [[nodiscard]] usize get_btns_size() const noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void locale_updated(const Renderer& renderer) noexcept override;
  void input(const event::Input& evt, InputData& data) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

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

