/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-08
 *==========================*/

#ifndef PXL_VIEW_SDL3_MANAGER_HPP
#define PXL_VIEW_SDL3_MANAGER_HPP

#include "./box/box.hpp"
#include "./box/draw.hpp"
#include "./box/menu.hpp"
#include "./box/status.hpp"
#include "./box/tool.hpp"
#include "./font.hpp"
#include "./modal/file.hpp"
#include "./modal/modal.hpp"
#include "./renderer.hpp"
#include "./texture.hpp"
#include "./widget/button.hpp"
#include "./widget/textbox.hpp"
#include "./window.hpp"
#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "core/ds/vector.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"
#include "view/modal.hpp"

namespace view::sdl3 {

class Manager {
public:
  Manager() noexcept = default;
  Manager(const Manager&) noexcept = delete;
  Manager& operator=(const Manager&) noexcept = delete;
  Manager(Manager&&) noexcept = delete;
  Manager& operator=(Manager&&) noexcept = delete;

  void init() noexcept;
  ~Manager() noexcept;

  [[nodiscard]] i32 get_window_width() const noexcept;
  [[nodiscard]] i32 get_window_height() const noexcept;
  [[nodiscard]] const frect& get_draw_rect() const noexcept;
  [[nodiscard]] const frect& get_canvas_rect() const noexcept;

  void set_draw_size(ivec size) noexcept;
  void set_canvas_rect(const frect& canvas_rect) noexcept;
  void set_cursor_canvas_pos(ivec pos) noexcept;

  // NOTE: Might add index or id to reference which drawbox
  [[nodiscard]] Texture& get_bg_texture() noexcept;
  [[nodiscard]] Texture& get_bot_texture() noexcept;
  [[nodiscard]] Texture& get_curr_texture() noexcept;
  [[nodiscard]] Texture& get_empty_texture() noexcept;
  [[nodiscard]] Texture& get_top_texture() noexcept;
  [[nodiscard]] Texture& get_select1_texture() noexcept;
  [[nodiscard]] Texture& get_select2_texture() noexcept;

  [[nodiscard]] void* get_modal_data(modal::Id id) const noexcept;

  // === Modifiers === //
  void push_modal(modal::Id id) noexcept;
  modal::Id pop_modal() noexcept;
  void clear_modals() noexcept;

  void run() noexcept;

private:
  Window window{};
  Renderer renderer{};

  Font font{};

  // NOTE: There can be multiple drawbox
  widget::DrawBox draw_box{};
  widget::ToolBox tool_box{};
  widget::StatusBox status_box{};
  widget::MenuBox menu_box{};

  ds::vector<widget::Box*> boxes{};
  // NOTE: Better to use a stack here
  ds::vector<widget::Modal*> modals{};

  event::Input input_evt{};
  i32 select_ticks = 0;
  i32 mouse_box_id = -1; // where is the mouse currently

  bool is_input_evt = false;
  bool running = false;

  void input() noexcept;

  void inline handle_mouse_input(
      const SDL_MouseButtonEvent& mouse, input::MouseState state
  ) noexcept;
  void handle_mouse_motion_input(const SDL_MouseButtonEvent& mouse) noexcept;
  void handle_mouse_scroll_input(const SDL_MouseWheelEvent& mouse) noexcept;

  inline void handle_key_down_input(i32 keycode) noexcept;
  inline void handle_key_up_input(i32 keycode) noexcept;

  void handle_resize(ivec new_size) noexcept;

  void update() noexcept;
  void render() noexcept;
};

} // namespace view::sdl3

#endif

