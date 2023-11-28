/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-17
 *==========================*/

#include "./manager.hpp"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "core/logger/logger.hpp"
#include "presenter/presenter.hpp"

namespace view::sdl3 {

inline void update_mouse_state(input::MouseState& state, bool& flag) noexcept {
  switch (state) {
  case input::MouseState::DOWN:
    state = input::MouseState::HOLD;
    flag = true;
    break;

  case input::MouseState::UP:
    state = input::MouseState::NONE;
    flag = true;
    break;

  default:
    // Do nothing UwU
    break;
  }
}

void Manager::input() noexcept {
  SDL_Event event{};
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      this->running = false;
      break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      this->handle_mouse_input(event.button, input::MouseState::DOWN);
      this->is_input_evt = true;
      break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      this->handle_mouse_input(event.button, input::MouseState::UP);
      this->is_input_evt = true;
      break;

    case SDL_EVENT_MOUSE_MOTION:
      this->handle_mouse_motion_input(event.button);
      this->is_input_evt = true;
      break;

    case SDL_EVENT_MOUSE_WHEEL:
      this->handle_mouse_scroll_input(event.wheel);
      this->is_input_evt = true;
      break;

    case SDL_EVENT_TEXT_INPUT:
      if (!SDL_TextInputActive()) {
        break;
      }
      this->data.sel_textbox->push_text(event.text.text);
      this->is_text_input_changed = true;
      break;

    case SDL_EVENT_TEXT_EDITING:
      // TODO:
      break;

    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
      if (this->mouse_box_id > -1) {
        this->boxes[this->mouse_box_id]->reset();
        this->mouse_box_id = -1;
      }
      break;

    case SDL_EVENT_KEY_DOWN:
      this->handle_key_down_input(event.key.keysym.sym);
      break;

    case SDL_EVENT_KEY_UP:
      this->handle_key_up_input(event.key.keysym.sym);
      break;

    case SDL_EVENT_WINDOW_RESIZED:
      this->handle_resize({event.window.data1, event.window.data2});
      break;
    }
  }

  this->handle_input_event();

  update_mouse_state(this->input_evt.mouse.left, this->is_input_evt);
  update_mouse_state(this->input_evt.mouse.right, this->is_input_evt);
  update_mouse_state(this->input_evt.mouse.middle, this->is_input_evt);
  this->input_evt.mouse.wheel = {};

  if (this->is_text_input_changed && this->data.sel_textbox) {
    this->data.sel_textbox->reposition_text_rect(this->renderer);
  }
}

void Manager::handle_input_event() noexcept {
  if (!this->is_input_evt) {
    return;
  }
  this->is_input_evt = false;

  if (this->locale_btn.rect.has_point(this->input_evt.mouse.pos)) {
    this->locale_btn.input(this->input_evt, this->data);
    return;
  }
  this->locale_btn.reset();

  if (this->ctx_menu_idx > -1) {
    this->handle_ctx_menu_event();
    return;
  }

  if (this->modals.get_size() > 0) {
    // NOTE: Check if only handle inputs on the top
    this->modals.back()->input(this->input_evt, this->data);
    if (this->data.new_sel_textbox) {
      if (this->data.sel_textbox) {
        this->data.sel_textbox->focused = false;
        this->data.sel_textbox->update_texture(this->renderer);
      }
      this->data.sel_textbox = this->data.new_sel_textbox;
      this->data.new_sel_textbox = nullptr;
      SDL_StartTextInput();
    } else if (this->input_evt.mouse.left == input::MouseState::UP && this->data.sel_textbox) {
      this->data.sel_textbox->focused = false;
      this->data.sel_textbox->update_texture(this->renderer);
      this->data.sel_textbox = nullptr;
      SDL_StopTextInput();
    }
    return;
  }

  // Only handle inputs on draw box when it was initially clicked
  if (this->is_draw_box_clicked) {
    this->draw_box.input(this->input_evt, this->data);
    if (this->input_evt.is_mouse_state(input::MouseState::UP)) {
      this->is_draw_box_clicked = false;
      /* this->draw_box.reset(); */
      this->mouse_box_id = -1;
    }
    return;
  }
  //

  i32 i = 0;
  for (; i < this->boxes.get_size(); ++i) {
    if (this->boxes[i]->rect.has_point(this->input_evt.mouse.pos)) {
      if (this->mouse_box_id == -1) {
        this->mouse_box_id = i;
      } else if (this->mouse_box_id != i) {
        this->boxes[this->mouse_box_id]->reset();
        this->mouse_box_id = i;
      }

      this->boxes[i]->input(this->input_evt, this->data);
      break;
    }
  }

  if (i == 0) { // Draw box index
    this->is_draw_box_clicked =
        this->input_evt.is_mouse_state(input::MouseState::DOWN);
  } else if (i == this->boxes.get_size()) {
    this->boxes[this->mouse_box_id]->reset();
    this->mouse_box_id = -1;
  }
}

// === Mouse Input === //

void Manager::handle_mouse_input(
    const SDL_MouseButtonEvent& mouse, input::MouseState state
) noexcept {
  this->input_evt.mouse.pos = {mouse.x, mouse.y};

  switch (mouse.button) {
  case SDL_BUTTON_LEFT:
    this->input_evt.mouse.left = state;
    break;

  case SDL_BUTTON_RIGHT:
    this->input_evt.mouse.right = state;
    break;

  case SDL_BUTTON_MIDDLE:
    this->input_evt.mouse.middle = state;
    break;

  default:
    // Do nothing
    break;
  }
}

void Manager::handle_mouse_motion_input(const SDL_MouseButtonEvent& mouse
) noexcept {
  this->input_evt.mouse.pos = {mouse.x, mouse.y};
}

void Manager::handle_mouse_scroll_input(const SDL_MouseWheelEvent& mouse
) noexcept {
  this->input_evt.mouse.pos = {mouse.mouseX, mouse.mouseY};
  this->input_evt.mouse.wheel = {mouse.x, mouse.y};
}

// === Mouse Event === //

[[nodiscard]] inline bool
has_mouse_event(bool flag, input::MouseState state) noexcept {
  return flag && state != input::MouseState::NONE;
}

inline void next_mouse_state(input::MouseState& state) noexcept {
  switch (state) {
  case input::MouseState::DOWN:
    state = input::MouseState::HOLD;
    break;

  case input::MouseState::UP:
    state = input::MouseState::NONE;
    break;

  default:
    // Do nothing
    break;
  }
}

// === Keyboard === //
void Manager::handle_key_down_input(i32 keycode) noexcept { // NOLINT
  if (SDL_TextInputActive()) {
    this->handle_key_down_text_input(keycode);
  } else {
    this->handle_key_down_key(keycode);
  }
}

void Manager::handle_key_down_key(i32 keycode) noexcept {
  switch (keycode) {
  case SDLK_LCTRL:
  case SDLK_RCTRL:
    this->input_evt.key.mods.ctrl = true;
    break;

  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    this->input_evt.key.mods.shift = true;
    break;

  case SDLK_LALT:
  case SDLK_RALT:
    this->input_evt.key.mods.alt = true;
    break;

  default:
    presenter::key_down_event(
        (input::Keycode)keycode, this->input_evt.key.mods
    );
    break;
  }
}

void Manager::handle_key_down_text_input(i32 keycode) noexcept {
  if (!this->data.sel_textbox) {
    return;
  }

  switch (keycode) {
  case SDLK_BACKSPACE:
    if (this->data.sel_textbox->pop_char() != '\0') {
      this->is_text_input_changed = true;
    }
    break;

  case SDLK_RETURN:
    this->data.sel_textbox->focused = false;
    this->data.sel_textbox->update_texture(this->renderer);
    this->data.sel_textbox = nullptr;
    this->is_text_input_changed = false;
    SDL_StopTextInput();
    break;

    /* case SDLK_TAB: */
    // TODO: Refocus to another input
    /* break; */

  default:
    break;
  }
}

void Manager::handle_key_up_input(i32 keycode) noexcept { // NOLINT
  if (SDL_TextInputActive()) {
    return;
  }

  switch (keycode) {
  case SDLK_LCTRL:
  case SDLK_RCTRL:
    this->input_evt.key.mods.ctrl = false;
    break;

  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    this->input_evt.key.mods.shift = false;
    break;

  case SDLK_LALT:
  case SDLK_RALT:
    this->input_evt.key.mods.alt = false;
    break;

  default:
    // Do nothing
    break;
  }
}

void Manager::handle_resize(ivec new_size) noexcept {
  this->window.size = new_size;

  f32 text_height = this->renderer.get_text_height();
  f32 menu_box_height = text_height + 4.0F;

  this->menu_box.resize({0.0F, 0.0F, (f32)new_size.x, menu_box_height});
  this->status_box.resize(
      {0.0F, (f32)new_size.y - text_height, (f32)new_size.x, text_height}
  );
  this->tool_box.resize(
      {0.0F, menu_box_height, 32.0F,
       (f32)new_size.y - menu_box_height - text_height}
  );

  this->timeline_box.resize(
      {32.0F, status_box.y - 200.0F, (f32)new_size.x - 32.0F, 200.0F}
  );

  this->draw_box.resize(
      {32.0F, menu_box_height, (f32)new_size.x - 32.0F,
       (f32)new_size.y - menu_box_height - text_height - this->timeline_box.h}
  );

  presenter::window_resized();
}

// Other events

void Manager::locale_updated() noexcept {
  this->renderer.locale_updated();

  for (usize i = 0; i < this->boxes.get_size(); ++i) {
    this->boxes[i]->locale_updated(this->renderer);
  }

  for (usize i = 0; i < this->modals.get_size(); ++i) {
    this->modals[i]->locale_updated(this->renderer);
  }

  // NOTE: boxes may have changed dimensions
  this->handle_resize(this->window.size);

  this->handle_ctx_menu_locale_updated();
}

} // namespace view::sdl3

