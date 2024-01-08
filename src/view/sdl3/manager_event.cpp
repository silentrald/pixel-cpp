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
      for (auto* ptr = event.text.text; *ptr != '\0'; ++ptr) {
        this->keypress_evt.push_char(*ptr);
      }
      break;

    case SDL_EVENT_TEXT_EDITING:
      // TODO:
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
  this->handle_key_event();

  if (this->input_evt.mouse.left == input::MouseState::UP) {
    data::set_left_click_widget(nullptr);
  }

  update_mouse_state(this->input_evt.mouse.left, this->is_input_evt);
  update_mouse_state(this->input_evt.mouse.right, this->is_input_evt);
  update_mouse_state(this->input_evt.mouse.middle, this->is_input_evt);

  this->input_evt.mouse.wheel = {};
  this->keypress_evt.reset();
}

void Manager::handle_input_event() noexcept {
  if (!this->is_input_evt) {
    return;
  }
  this->is_input_evt = false;

  // NOTE: Remove this once their modals are created
  this->fg_color.input(this->input_evt);
  this->bg_color.input(this->input_evt);

  if (this->locale_btn.rect.has_point(this->input_evt.mouse.pos)) {
    this->locale_btn.input(this->input_evt);
    if (data::is_left_click()) {
      presenter::set_locale();
    }
    return;
  }
  this->locale_btn.reset();

  if (this->ctx_menu_idx > -1) {
    this->handle_ctx_menu_event();
    return;
  }

  if (this->modals.get_size() > 0) {
    // NOTE: Check if only handle inputs on the top
    this->modals.back()->input(this->input_evt);
    return;
  }

  for (i32 i = 0; i < this->boxes.get_size(); ++i) {
    this->boxes[i]->input(this->input_evt);
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

void Manager::handle_key_event() noexcept {
  if (this->keypress_evt.has_keys()) {
    while (this->keypress_evt.has_next()) {
      if (data::get_selected_input()) {
        data::get_selected_input()->key_input(this->keypress_evt);
        if (this->keypress_evt.get_prev_char() == input::Keycode::TAB) {
          data::set_selected_input(data::get_selected_input()->next_input);
          if (data::get_selected_input()) {
            data::get_selected_input()->focused = true;
          } else {
            SDL_StopTextInput();
          }
        }
        continue;
      }

      // Check for tab inputs then switch to first input
      if (data::get_first_input() == nullptr) {
        break;
      }

      while (this->keypress_evt.has_next()) {
        if (this->keypress_evt.get_next_char() == input::Keycode::TAB) {
          data::set_selected_input(data::get_first_input());
          data::get_selected_input()->focused = true;
          SDL_StartTextInput();
          break;
        }
      }

      if (data::get_selected_input() == nullptr) {
        SDL_StopTextInput();
        break;
      }
    }
  }

  if (data::get_new_selected_input()) {
    if (data::get_selected_input()) {
      data::get_selected_input()->unfocused();
    }
    data::set_selected_input(data::get_new_selected_input());
    data::get_selected_input()->focused = true;

    data::set_new_selected_input(nullptr);
    SDL_StartTextInput();
  } else if ((data::is_clear_selected() || this->input_evt.mouse.left == input::MouseState::UP) && data::get_selected_input()) {
    data::set_clear_selected(false);
    data::get_selected_input()->unfocused();
    data::set_selected_input(nullptr);
    SDL_StopTextInput();
  }
}

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

  case SDLK_TAB:
    this->keypress_evt.push_char(input::Keycode::TAB);
    break;

  default:
    presenter::key_down_event(
        (input::Keycode)keycode, this->input_evt.key.mods
    );
    break;
  }
}

void Manager::handle_key_down_text_input(i32 keycode) noexcept {
  if (!data::get_selected_input()) {
    return;
  }

  switch (keycode) {
  case SDLK_BACKSPACE:
    this->keypress_evt.push_char(input::Keycode::BACKSPACE);
    break;

  case SDLK_RETURN:
    this->keypress_evt.push_char(input::Keycode::ENTER);
    SDL_StopTextInput();
    break;

  case SDLK_TAB:
    this->keypress_evt.push_char(input::Keycode::TAB);
    break;

  case SDLK_ESCAPE:
    data::set_clear_selected(true);
    break;

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

  f32 text_height = renderer::get_text_height();
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

void Manager::update_locale() noexcept {
  renderer::update_locale();

  for (usize i = 0; i < this->boxes.get_size(); ++i) {
    this->boxes[i]->update_locale();
  }

  for (usize i = 0; i < this->modals.get_size(); ++i) {
    this->modals[i]->update_locale();
  }

  // TODO: Temp
  this->fg_color.update_locale();
  this->bg_color.update_locale();

  // NOTE: boxes may have changed dimensions
  this->handle_resize(this->window.size);

  this->handle_ctx_menu_update_locale();
}

} // namespace view::sdl3
