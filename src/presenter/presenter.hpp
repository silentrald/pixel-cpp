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

namespace presenter {

void init() noexcept;
void run() noexcept;

// Events

void window_resized() noexcept;

/**
 * @param keycode
 **/
void key_down_event(input::Keycode keycode, input::KeyMod key_mod) noexcept;

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

void close_modals() noexcept;
void new_file_clicked() noexcept;
void create_anim() noexcept;

// NOTE: For debugging purposes
void debug_callback() noexcept;

} // namespace presenter

#endif

