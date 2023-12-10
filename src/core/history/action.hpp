/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#ifndef MODULES_HISTORY_ACTION_HPP
#define MODULES_HISTORY_ACTION_HPP

#include "./arena.hpp"
#include "core/draw/types.hpp"
#include "model/model.hpp"
#include <cstring>

namespace history {

enum ActionType {
  NONE,
  //
  EDIT_IMAGE,
  INSERT_LAYER,
  SET_VISIBILITY,
  CHANGE_SELECTION,
};

struct EditImageAction {
  // Pointers can be disjointed
  draw::data_ptr prev_pixels = nullptr;
  draw::data_ptr pixels = nullptr;
};

struct InsertLayerAction {
  usize prev_layer_index = 0U; // prev layer_index
  usize layer_index = 0U;      // created layer_index
};

struct SetVisibilityAction {
  usize layer_index = 0U;
  bool visibility = false;
};

struct ChangeSelectionAction {
  usize prev_frame_index = 0U;
  usize prev_layer_index = 0U;
  usize frame_index = 0U;
  usize layer_index = 0U;
};

struct Action {
  ActionType type = ActionType::NONE;
  union {
    EditImageAction edit_image;
    InsertLayerAction insert_layer;
    SetVisibilityAction set_visibility;
    ChangeSelectionAction change_selection;
  };

  Action() noexcept {};
  explicit Action(ActionType type) noexcept : type(type) {}
  Action(const Action& rhs) noexcept;
  Action& operator=(const Action& rhs) noexcept;
  Action(Action&& rhs) noexcept;
  Action& operator=(Action&& rhs) noexcept;
  ~Action() noexcept {};

  void undo(Model& model) const noexcept;
  void redo(Model& model) const noexcept;

  [[nodiscard]] void* get_start_ptr() const noexcept;
  [[nodiscard]] void* get_start_ptr(ActionType last_type) const noexcept;
};

} // namespace history

#endif

