/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-18
 *===============================*/

#ifndef HISTORY_ACTION_HPP
#define HISTORY_ACTION_HPP

#include "./action/edit_image.hpp"
#include "./action/insert_frame.hpp"
#include "./action/insert_layer.hpp"
#include "./action/remove_layer.hpp"
#include "./action/set_visibility.hpp"
#include "./arena.hpp"
#include "core/draw/types.hpp"
#include "model/model.hpp"
#include <cstring>

namespace history {

enum class ActionType {
  NONE,
  //
  EDIT_IMAGE,
  INSERT_LAYER,
  REMOVE_LAYER,
  INSERT_FRAME,
  SET_VISIBILITY,
};

struct Action {
  ActionType type = ActionType::NONE;
  union {
    EditImage edit_image;
    InsertLayer insert_layer;
    RemoveLayer remove_layer;
    InsertFrame insert_frame;
    SetVisibility set_visibility;
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
  [[nodiscard]] void* get_start_ptr(const Action& last_action) const noexcept;
};

} // namespace history

#endif

