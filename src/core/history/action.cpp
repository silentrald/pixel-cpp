/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#include "./action.hpp"
#include "core/logger/logger.hpp"
#include <cstdlib>

namespace history {

// NOTE: Bug here
Action::Action(const Action& rhs) noexcept {
  // NOLINTNEXTLINE
  std::memcpy(this, &rhs, sizeof(Action));
}

Action& Action::operator=(const Action& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  // NOLINTNEXTLINE
  std::memcpy(this, &rhs, sizeof(Action));

  return *this;
}

Action::Action(Action&& rhs) noexcept : type(rhs.type) {
  // NOLINTNEXTLINE
  std::memcpy(this, &rhs, sizeof(Action));
}

Action& Action::operator=(Action&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  // NOLINTNEXTLINE
  std::memcpy(this, &rhs, sizeof(Action));

  return *this;
}

// NOLINTNEXTLINE
#define ACTION_UNDO(type, var)                                                 \
  case ActionType::type:                                                       \
    this->var.undo(model);                                                     \
    break;

void Action::undo(Model& model) const noexcept {
  switch (this->type) {
    ACTION_UNDO(EDIT_IMAGE, edit_image)
    ACTION_UNDO(INSERT_LAYER, insert_layer)
    ACTION_UNDO(REMOVE_LAYER, remove_layer)
    ACTION_UNDO(INSERT_FRAME, insert_frame)
    ACTION_UNDO(REMOVE_FRAME, remove_frame)
    ACTION_UNDO(SET_VISIBILITY, set_visibility)

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }
}

#undef ACTION_UNDO

// NOLINTNEXTLINE
#define ACTION_REDO(type, var)                                                 \
  case ActionType::type:                                                       \
    this->var.redo(model);                                                     \
    break;

void Action::redo(Model& model) const noexcept {
  switch (this->type) {
    ACTION_REDO(EDIT_IMAGE, edit_image)
    ACTION_REDO(INSERT_LAYER, insert_layer)
    ACTION_REDO(REMOVE_LAYER, remove_layer)
    ACTION_REDO(INSERT_FRAME, insert_frame)
    ACTION_REDO(REMOVE_FRAME, remove_frame)
    ACTION_REDO(SET_VISIBILITY, set_visibility)

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }
}

#undef ACTION_REDO

void* Action::get_start_ptr() const noexcept {
  switch (this->type) {
  case ActionType::EDIT_IMAGE:
    return this->edit_image.prev_pixels;

  case ActionType::REMOVE_LAYER:
    return this->remove_layer.data;

  case ActionType::REMOVE_FRAME:
    return this->remove_frame.data;

  case ActionType::INSERT_LAYER:
  case ActionType::INSERT_FRAME:
  case ActionType::SET_VISIBILITY:
    return nullptr;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }

  // For clang-tidy
  return nullptr;
}

void* Action::get_start_ptr(const Action& last_action) const noexcept {
  switch (this->type) {
  case ActionType::EDIT_IMAGE:
    // Check if image action and the same frame and layer
    return (last_action.type == ActionType::EDIT_IMAGE &&
            last_action.edit_image.layer_index ==
                this->edit_image.layer_index &&
            last_action.edit_image.frame_index == this->edit_image.frame_index)
               ? this->edit_image.pixels
               : this->edit_image.prev_pixels;

  case ActionType::REMOVE_LAYER:
    return this->remove_layer.data;

  case ActionType::REMOVE_FRAME:
    return this->remove_frame.data;

  case ActionType::INSERT_LAYER:
  case ActionType::INSERT_FRAME:
  case ActionType::SET_VISIBILITY:
    return nullptr;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }

  // For clang-tidy
  return nullptr;
}

} // namespace history

