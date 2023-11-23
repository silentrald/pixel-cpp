/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#include "./caretaker.hpp"
#include "core/logger/logger.hpp"
#include "types.hpp"
#include <cstdlib>
#include <cstring>
#include <utility>

namespace history {

Caretaker::Caretaker() noexcept
    : cursor(-1), start_cursor(-1), end_cursor(-1) {}

error_code Caretaker::init(i32 capacity) noexcept {
  TRY(this->actions.resize(capacity));

  // 5mb for now
  TRY(this->arena.init(1024U * 1024U * 5U));

  this->cursor = this->start_cursor = this->end_cursor = -1;

  return error_code::OK;
}

void Caretaker::prepare_push_action() noexcept {
  if (this->cursor == -1) {
    this->arena.reset();
  } else if (this->cursor != this->end_cursor) {
    this->move_arena_cursor();
  }
}

void Caretaker::push_action(Action&& action) noexcept {
  if (this->cursor == -1) {
    this->cursor = 0;
    this->start_cursor = 0;
    this->end_cursor = 0;

    this->actions[0] = std::move(action);
    return;
  }

  // Move the cursor and move the new snapshot
  ++this->cursor;
  if (this->cursor == this->actions.get_size()) {
    this->cursor = 0;
  }
  this->actions[this->cursor] = std::move(action);
  this->end_cursor = this->cursor;

  if (this->start_cursor == this->end_cursor) {
    ++this->start_cursor;
    if (this->start_cursor == this->actions.get_size()) {
      this->start_cursor = 0;
    }
  }
}

Action Caretaker::create_edit_image_action(usize bytes) noexcept {
  void* orig_ptr = this->arena.get_cursor_data();
  Action action{ActionType::EDIT_IMAGE};

  if (this->cursor > -1 &&
      this->actions[this->cursor].type == ActionType::EDIT_IMAGE) {
    action.edit_image.prev_pixels =
        this->actions[this->cursor].edit_image.pixels;
  } else {
    action.edit_image.prev_pixels = (draw::data_ptr)this->arena.allocate(bytes);
    this->move_start_cursor(orig_ptr);
    orig_ptr = this->arena.get_cursor_data();
  }

  action.edit_image.pixels = (draw::data_ptr)this->arena.allocate(bytes);
  this->move_start_cursor(orig_ptr);

  return action;
}

void Caretaker::move_start_cursor(void* orig_ptr) noexcept {
  if (this->start_cursor == -1) {
    return;
  }

  auto* new_ptr = this->arena.get_cursor_data();
  if (orig_ptr == new_ptr) {
    return;
  }

  void* curr_ptr = nullptr;
  if (orig_ptr < new_ptr) {
    this->invalidate_actions(orig_ptr, new_ptr);
    return;
  }

  this->invalidate_actions_end(orig_ptr);
  this->invalidate_actions_start(new_ptr);
}

void Caretaker::invalidate_actions(void* start_ptr, void* end_ptr) noexcept {
  void* ptr = nullptr;
  if (this->start_cursor <= this->end_cursor) {
    for (i32 i = this->start_cursor; i <= this->end_cursor; ++i) {
      ptr = this->actions[i].get_start_ptr();
      if (ptr == nullptr) {
        continue;
      }

      if (ptr < start_ptr || ptr >= end_ptr) {
        return;
      }
      this->start_cursor = i;
    }
    return;
  }

  for (i32 i = this->start_cursor; i < this->actions.get_size(); ++i) {
    ptr = this->actions[i].get_start_ptr();
    if (ptr == nullptr) {
      continue;
    }

    if (ptr < start_ptr || ptr >= end_ptr) {
      return;
    }
    this->start_cursor = i;
  }

  for (i32 i = 0; i <= this->end_cursor; ++i) {
    ptr = this->actions[i].get_start_ptr();
    if (ptr == nullptr) {
      continue;
    }

    if (ptr < start_ptr || ptr >= end_ptr) {
      return;
    }
    this->start_cursor = i;
  }
}

// Move the start_cursor no matter what
void Caretaker::invalidate_actions_end(void* start_ptr) noexcept {
  void* ptr = nullptr;
  if (this->start_cursor <= this->end_cursor) {
    for (; this->start_cursor <= this->end_cursor; ++this->start_cursor) {
      ptr = this->actions[this->start_cursor].get_start_ptr();
      if (ptr != nullptr && ptr < start_ptr) {
        return;
      }
    }
    return;
  }

  for (; this->start_cursor < this->actions.get_size(); ++this->start_cursor) {
    ptr = this->actions[this->start_cursor].get_start_ptr();
    if (ptr != nullptr && ptr < start_ptr) {
      return;
    }
  }

  for (this->start_cursor = 0; this->start_cursor <= this->end_cursor;
       ++this->start_cursor) {
    ptr = this->actions[this->start_cursor].get_start_ptr();
    if (ptr != nullptr && ptr < start_ptr) {
      return;
    }
  }
}

void Caretaker::invalidate_actions_start(void* end_ptr) noexcept {
  void* ptr = nullptr;
  if (this->start_cursor <= this->end_cursor) {
    for (i32 i = this->start_cursor; i <= this->end_cursor; ++i) {
      ptr = this->actions[this->start_cursor].get_start_ptr();
      if (ptr == nullptr) {
        continue;
      }

      if (ptr >= end_ptr) {
        return;
      }
      this->start_cursor = i;
    }
    return;
  }

  for (i32 i = this->start_cursor; i < this->actions.get_size(); ++i) {
    ptr = this->actions[this->start_cursor].get_start_ptr();
    if (ptr != nullptr) {
      continue;
    }

    if (ptr >= end_ptr) {
      return;
    }
    this->start_cursor = i;
  }

  for (i32 i = 0; i <= this->end_cursor; ++i) {
    ptr = this->actions[this->start_cursor].get_start_ptr();
    if (ptr == nullptr) {
      continue;
    }

    if (ptr >= end_ptr) {
      return;
    }
    this->start_cursor = i;
  }
}

void Caretaker::move_arena_cursor() noexcept {
  void* ptr = nullptr;
  i32 next_cursor = 0;
  for (auto curr_cursor = this->cursor; curr_cursor != this->start_cursor;
       --curr_cursor) {
    if (curr_cursor == -1) {
      curr_cursor = this->actions.get_size() - 1;
      next_cursor = 0;
    } else {
      next_cursor = curr_cursor + 1;
    }

    ptr = this->actions[next_cursor].get_start_ptr(this->actions[cursor].type);

    if (ptr == nullptr) {
      continue;
    }

    this->arena.set_cursor_with_ptr(ptr);
    break;
  }
}

bool Caretaker::can_undo() const noexcept {
  return this->cursor != -1;
}

bool Caretaker::undo(Model& model) noexcept {
  if (!this->can_undo()) {
    return false;
  }

  this->actions[this->cursor].undo(model);

  if (this->cursor == this->start_cursor) {
    this->cursor = -1;
  } else if (this->cursor == 0) {
    this->cursor = this->actions.get_size() - 1;
  } else {
    --this->cursor;
  }

  return true;
}

bool Caretaker::can_redo() const noexcept {
  return this->cursor != this->end_cursor;
}

bool Caretaker::redo(Model& model) noexcept {
  if (!this->can_redo()) {
    return false;
  }

  if (this->cursor == -1) {
    this->cursor = this->start_cursor;
  } else {
    ++this->cursor;
    if (this->cursor == this->actions.get_size()) {
      this->cursor = 0;
    }
  }

  this->actions[this->cursor].redo(model);

  return true;
}

} // namespace history
