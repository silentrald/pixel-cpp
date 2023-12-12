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
  std::memcpy(&this->image, &rhs.image, sizeof(Action) - sizeof(ActionType));
}

Action& Action::operator=(const Action& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->type = rhs.type;
  std::memcpy(&this->image, &rhs.image, sizeof(Action) - sizeof(ActionType));

  return *this;
}

Action::Action(Action&& rhs) noexcept : type(rhs.type) {
  std::memcpy(&this->image, &rhs.image, sizeof(Action) - sizeof(ActionType));
}

Action& Action::operator=(Action&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->type = rhs.type;
  std::memcpy(&this->image, &rhs.image, sizeof(Action) - sizeof(ActionType));

  return *this;
}

void Action::undo(Model& model) const noexcept {
  switch (this->type) {
  case ActionType::ADD_IMAGE: {
    TRY_ABORT(
        model.anim.remove_image(
            this->image.frame_index, this->image.layer_index
        ),
        "Anim remove image"
    );
    model.frame_index = this->image.frame_index;
    model.layer_index = this->image.layer_index;
    model.img_id = 0U;
  } break;

  case ActionType::EDIT_IMAGE:
    // Try to get the img_id if it matches, else update the model.img
    {
      auto id = model.anim.get_image_id(
          this->image.frame_index, this->image.layer_index
      );
      if (model.img_id != id) {
        model.img_id = id;
        model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Anim read");
      }
    }

    model.img.set_pixels(
        this->image.prev_pixels, model.anim.get_image_bytes_size()
    );
    TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");

    model.layer_index = this->image.layer_index;
    model.frame_index = this->image.frame_index;
    break;

  case ActionType::INSERT_LAYER:
    TRY_ABORT(
        model.anim.remove_layer(this->insert_layer.layer_index),
        "Anim remove layer"
    );
    model.layer_index = this->insert_layer.prev_layer_index;
    model.img_id =
        model.anim.get_image_id(model.frame_index, model.layer_index);
    if (model.img_id > 0U) {
      model.img =
          *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
    }
    break;

  case ActionType::INSERT_FRAME:
    TRY_ABORT(
        model.anim.remove_frame(this->insert_frame.frame_index),
        "Anim remove frame"
    );
    model.frame_index = this->insert_frame.prev_frame_index;
    model.img_id =
        model.anim.get_image_id(model.frame_index, model.layer_index);
    if (model.img_id > 0U) {
      model.img =
          *TRY_ABORT_RET(model.anim.get_image(model.img_id), "Anim Read");
    }
    break;

  case ActionType::SET_VISIBILITY:
    model.anim.set_layer_visibility(
        this->set_visibility.layer_index, !this->set_visibility.visibility
    );
    break;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }
}

void Action::redo(Model& model) const noexcept {
  switch (this->type) {
  case ActionType::ADD_IMAGE: {
    auto id = *TRY_ABORT_RET(
        model.anim.create_image(
            this->image.frame_index, this->image.layer_index
        ),
        "Anim write"
    );
    model.img_id = id;
    model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Anim read");

    model.img.set_pixels(this->image.pixels, model.anim.get_image_bytes_size());
    TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");

    model.layer_index = this->image.layer_index;
    model.frame_index = this->image.frame_index;
  } break;

  case ActionType::EDIT_IMAGE:
    // Try to get the img_id if its correct
    {
      auto id = model.anim.get_image_id(
          this->image.frame_index, this->image.layer_index
      );
      if (model.img_id != id) {
        model.img_id = id;
        model.img = *TRY_ABORT_RET(model.anim.get_image(id), "Anim read");
      }
    }

    model.img.set_pixels(this->image.pixels, model.anim.get_image_bytes_size());
    TRY_ABORT(model.anim.write_image_to_disk(model.img), "Anim disk write");

    model.layer_index = this->image.layer_index;
    model.frame_index = this->image.frame_index;
    break;

  case ActionType::INSERT_LAYER:
    model.layer_index = this->insert_layer.layer_index;
    TRY_ABORT(
        model.anim.insert_layer(this->insert_layer.layer_index),
        "Anim insert layer"
    );
    model.img_id = 0U;
    break;

  case ActionType::INSERT_FRAME:
    model.frame_index = this->insert_frame.frame_index;
    TRY_ABORT(
        model.anim.insert_frame(this->insert_frame.frame_index),
        "Anim insert frame"
    )
    model.img_id = 0U;
    break;

  case ActionType::SET_VISIBILITY:
    model.anim.set_layer_visibility(
        this->set_visibility.layer_index, this->set_visibility.visibility
    );
    break;

  case ActionType::NONE:
    logger::fatal("Invalid action");
    std::abort();
  }
}

void* Action::get_start_ptr() const noexcept {
  switch (this->type) {
  case ActionType::ADD_IMAGE:
  case ActionType::EDIT_IMAGE:
    return this->image.prev_pixels;

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
  case ActionType::ADD_IMAGE:
  case ActionType::EDIT_IMAGE:
    // Check if image action and the same frame and layer
    return ((last_action.type == ActionType::ADD_IMAGE ||
             last_action.type == ActionType::EDIT_IMAGE) &&
            last_action.image.layer_index == this->image.layer_index &&
            last_action.image.frame_index == this->image.frame_index)
               ? this->image.pixels
               : this->image.prev_pixels;

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

