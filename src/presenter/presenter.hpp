/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-09
 *==========================*/

#ifndef PXL_PRESENTER_HPP
#define PXL_PRESENTER_HPP

#include "types.hpp"
#include "view/event.hpp"
#include "view/input.hpp"

// NOTE: Can be changed depending on the gui to be used
#include "view/modal.hpp"
#include "view/sdl3/manager.hpp"
using View = view::sdl3::Manager;

class Model;

namespace presenter {

// === General Functions === //

void init() noexcept;
void run() noexcept;

// NOTE: For debugging purposes
void debug_callback() noexcept;

// === Unknown === //
// TODO: Add the param of the new value

void set_locale() noexcept;

void window_resized() noexcept;

/**
 * @param keycode
 **/
void key_down_event(input::Keycode keycode, input::KeyMod key_mod) noexcept;

void set_selected(u32 frame_id, i32 layer_index) noexcept;

void new_file_clicked() noexcept;

// === Context Menu Events === //

void toggle_file_ctx_menu() noexcept;
void toggle_edit_ctx_menu() noexcept;
void open_export_ctx_menu() noexcept;

void close_ctx_menus() noexcept;

// === Anim Mutations === //

void create_anim() noexcept;
void toggle_visibility(i32 layer_index) noexcept;
void insert_layer(i32 layer_index) noexcept;
void push_back_layer() noexcept;
void undo_action() noexcept;
void redo_action() noexcept;

// === Tools === //

/**
 * When a canvas box is interacted with a mouse event
 *
 * @param key
 * @param mouse
 * @param pos - position relative to the canvas
 **/
void canvas_mouse_event(const event::Input& evt) noexcept;

void set_pencil_tool() noexcept;
void set_eraser_tool() noexcept;
void set_line_tool() noexcept;
void set_fill_tool() noexcept;
void set_select_tool() noexcept;

// === Update View === //

void update_canvas_textures() noexcept;
void update_view() noexcept;
void close_modals() noexcept;

// === File Stuff === //

void save_file() noexcept;
void open_file() noexcept;
void export_to_png() noexcept;

} // namespace presenter

// Variables
namespace tool {

class Pencil;
class Eraser;
class Line;
class Fill;
class Select;
class Pan;
class Zoom;

} // namespace tool

namespace cfg {

class Shortcut;

} // namespace cfg

namespace history {

class Caretaker;

} // namespace history

namespace file {

class Pxl;
class Png;

} // namespace file

extern Model model_;
extern View view_;

extern tool::Pencil pencil_;
extern tool::Eraser eraser_;
extern tool::Line line_;
extern tool::Fill fill_;
extern tool::Select select_;

extern tool::Pan pan_;
extern tool::Zoom zoom_;

extern cfg::Shortcut shortcut_;

extern history::Caretaker caretaker_;

extern file::Pxl pxl_;
extern file::Png png_;

#endif

