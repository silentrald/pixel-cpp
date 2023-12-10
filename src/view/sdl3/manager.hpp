/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-08
 *==========================*/

#ifndef PXL_VIEW_SDL3_MANAGER_HPP
#define PXL_VIEW_SDL3_MANAGER_HPP

#include "../input.hpp"
#include "./box/box.hpp"
#include "./box/draw.hpp"
#include "./box/menu.hpp"
#include "./box/status.hpp"
#include "./box/timeline.hpp"
#include "./box/tool.hpp"
#include "./font.hpp"
#include "./input/button.hpp"
#include "./input/textbox.hpp"
#include "./modal/file.hpp"
#include "./modal/modal.hpp"
#include "./texture.hpp"
#include "./window.hpp"
#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "core/draw/anim.hpp"
#include "core/ds/vector.hpp"
#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"
#include "view/modal.hpp"
#include "view/sdl3/box/preview.hpp"
#include "view/sdl3/widget/color_picker.hpp"
#include "view/sdl3/widget/context_menu.hpp"

namespace view::sdl3 {

class Manager {
public:
  Manager() noexcept = default;
  Manager(const Manager&) noexcept = delete;
  Manager& operator=(const Manager&) noexcept = delete;
  Manager(Manager&&) noexcept = delete;
  Manager& operator=(Manager&&) noexcept = delete;

  [[nodiscard]] error_code init() noexcept;
  ~Manager() noexcept;

  [[nodiscard]] i32 get_window_width() const noexcept;
  [[nodiscard]] i32 get_window_height() const noexcept;
  [[nodiscard]] const frect& get_draw_rect() const noexcept;
  [[nodiscard]] const frect& get_canvas_rect() const noexcept;

  // NOTE: Might add index or id to reference which drawbox
  [[nodiscard]] Texture& get_bg_texture() noexcept;
  [[nodiscard]] Texture& get_bot_texture() noexcept;
  [[nodiscard]] Texture& get_curr_texture() noexcept;
  [[nodiscard]] Texture& get_empty_texture() noexcept;
  [[nodiscard]] Texture& get_top_texture() noexcept;
  [[nodiscard]] Texture& get_select1_texture() noexcept;
  [[nodiscard]] Texture& get_select2_texture() noexcept;

  [[nodiscard]] void* get_modal_data(modal::Id id) const noexcept;

  // === Setters ==== //

  void set_draw_size(ivec size) noexcept;
  void set_canvas_rect(const frect& canvas_rect) noexcept;
  void set_cursor_canvas_pos(ivec pos) noexcept;

  void set_fg_color(rgba8 color) noexcept;
  void set_bg_color(rgba8 color) noexcept;
  [[nodiscard]] error_code set_anim(const draw::Anim* anim) noexcept;
  void set_preview_playing(bool play) noexcept;

  // === Modifiers === //

  void locale_updated() noexcept;

  [[nodiscard]] error_code
  insert_layer(usize index, const draw::LayerInfo& layer_info) noexcept;
  void set_layer_visible(usize index, bool visible) noexcept;
  void clear_layers() noexcept;

  void set_active_on_timeline(usize frame_index, usize layer_index) noexcept;
  void set_frame_range(usize start_frame, usize end_frame) noexcept;

  [[nodiscard]] error_code push_modal(modal::Id id) noexcept;
  modal::Id pop_modal() noexcept;
  void clear_modals() noexcept;

  // === Context Menus === //

  void toggle_file_ctx_menu() noexcept;
  void toggle_edit_ctx_menu() noexcept;
  [[nodiscard]] error_code open_export_ctx_menu() noexcept;
  void open_layers_ctx_menu() noexcept;
  void open_timeline_ctx_menu() noexcept;
  void close_ctx_menus() noexcept;

  void run() noexcept;

private:
  Window window{};

  // NOTE: There can be multiple drawbox
  widget::DrawBox draw_box{};
  widget::TimelineBox timeline_box{};
  widget::ToolBox tool_box{};
  widget::StatusBox status_box{};
  widget::MenuBox menu_box{};
  widget::PreviewBox preview_box{};

  ds::vector<widget::Box*> boxes{};
  // NOTE: Better to use a stack here
  ds::vector<widget::Modal*> modals{};

  // NOTE: Only for level 1 ctx menu
  ds::vector<widget::ContextMenu> ctx_menus{};
  ds::vector<widget::ContextMenu> ctx_menu_stack{};

  // TODO: Change to dropdown, and store it in a box
  widget::Button locale_btn{};

  event::Input input_evt{};
  event::KeyPress keypress_evt{};
  InputData data{};

  widget::ColorPicker fg_color{};
  widget::ColorPicker bg_color{};

  i32 mouse_box_id = -1; // where is the mouse currently
  i32 ctx_menu_idx = -1; // which context menu is open

  bool running = false;
  bool is_input_evt = false;
  bool is_draw_box_clicked = false;

  void input() noexcept;
  void handle_input_event() noexcept;
  void handle_key_event() noexcept;
  void inline handle_mouse_input(
      const SDL_MouseButtonEvent& mouse, input::MouseState state
  ) noexcept;
  void handle_mouse_motion_input(const SDL_MouseButtonEvent& mouse) noexcept;
  void handle_mouse_scroll_input(const SDL_MouseWheelEvent& mouse) noexcept;

  inline void handle_key_down_input(i32 keycode) noexcept;
  inline void handle_key_down_key(i32 keycode) noexcept;
  inline void handle_key_down_text_input(i32 keycode) noexcept;
  inline void handle_key_up_input(i32 keycode) noexcept;

  void handle_resize(ivec new_size) noexcept;

  // === Context Menu === //

  [[nodiscard]] error_code init_ctx_menus() noexcept;
  void handle_ctx_menu_event() noexcept;
  void handle_ctx_menu_locale_updated() noexcept;

  void reset_data() noexcept;

  void update(f32 delta) noexcept;
  void render() noexcept;
};

} // namespace view::sdl3

#endif

