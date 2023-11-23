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

Action::Action(const Action& rhs) noexcept : type(rhs.type) {
  std::memcpy(
      &this->edit_image, &rhs.edit_image, sizeof(Action) - sizeof(ActionType)
  );
}

Action& Action::operator=(const Action& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->type = rhs.type;
  std::memcpy(
      &this->edit_image, &rhs.edit_image, sizeof(Action) - sizeof(ActionType)
  );

  return *this;
}

Action::Action(Action&& rhs) noexcept : type(rhs.type) {
  std::memcpy(
      &this->edit_image, &rhs.edit_image, sizeof(Action) - sizeof(ActionType)
  );
}

Action& Action::operator=(Action&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->type = rhs.type;
  std::memcpy(
      &this->edit_image, &rhs.edit_image, sizeof(Action) - sizeof(ActionType)
  );

  return *this;
}

void Action::undo(Model& model) const noexcept {
  switch (this->type) {
  case ActionType::EDIT_IMAGE:
    model.img.set_pixels(
        this->edit_image.prev_pixels, model.anim.get_image_bytes_size()
    );
    TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");
    break;

  case ActionType::INSERT_LAYER:
    TRY_ABORT(
        model.anim.remove_layer(this->insert_layer.layer_index),
        "Anim remove layer"
    );
    model.layer_index = this->insert_layer.prev_layer_index;
    model.img_id = model.anim.get_image_id(model.frame_id, model.layer_index);
    model.img = *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
    break;

  case ActionType::SET_VISIBILITY:
    model.anim.set_layer_visibility(
        this->set_visibility.layer_index, !this->set_visibility.visibility
    );
    break;

  case ActionType::CHANGE_SELECTION:
    model.frame_id = this->change_selection.prev_frame_id;
    model.layer_index = this->change_selection.prev_layer_index;
    model.img_id = model.anim.get_image_id(model.frame_id, model.layer_index);
    model.img = *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
    break;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }
}

void Action::redo(Model& model) const noexcept {
  switch (this->type) {
  case ActionType::EDIT_IMAGE:
    model.img.set_pixels(
        this->edit_image.pixels, model.anim.get_image_bytes_size()
    );
    TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");
    break;

  case ActionType::INSERT_LAYER:
    model.layer_index = this->insert_layer.layer_index;
    TRY_ABORT(
        model.anim.insert_layer(this->insert_layer.layer_index),
        "Anim insert layer"
    );
    model.img_id = model.anim.get_image_id(model.frame_id, model.layer_index);
    model.img = *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
    break;

  case ActionType::SET_VISIBILITY:
    model.anim.set_layer_visibility(
        this->set_visibility.layer_index, this->set_visibility.visibility
    );
    break;

  case ActionType::CHANGE_SELECTION:
    model.frame_id = this->change_selection.frame_id;
    model.layer_index = this->change_selection.layer_index;
    model.img_id = model.anim.get_image_id(model.frame_id, model.layer_index);
    model.img = *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
    break;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }
}

void* Action::get_start_ptr() const noexcept {
  switch (this->type) {
  case ActionType::EDIT_IMAGE:
    return this->edit_image.prev_pixels;

  case ActionType::INSERT_LAYER:
  case ActionType::SET_VISIBILITY:
  case ActionType::CHANGE_SELECTION:
    return nullptr;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }

  // For clang-tidy
  return nullptr;
}

void* Action::get_start_ptr(ActionType last_type) const noexcept {
  switch (this->type) {
  case ActionType::EDIT_IMAGE:
    return type == ActionType::EDIT_IMAGE ? this->edit_image.pixels
                                          : this->edit_image.prev_pixels;

  case ActionType::INSERT_LAYER:
  case ActionType::SET_VISIBILITY:
  case ActionType::CHANGE_SELECTION:
    return nullptr;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }

  // For clang-tidy
  return nullptr;
}

} // namespace history

