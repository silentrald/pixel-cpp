/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#include "./manager.hpp"
#include <cassert>
#include <new>

namespace view::sdl3 {

void* Manager::get_modal_data(modal::Id id) const noexcept {
  for (i32 i = this->modals.get_size() - 1; i >= 0; --i) {
    if (this->modals[i]->get_id() == id) {
      return this->modals[i]->get_data();
    }
  }

  logger::fatal("Could not get the modal data");
  std::abort();
}

error_code Manager::push_modal(modal::Id id) noexcept {
  assert(id != modal::Id::NONE);

  if (this->mouse_box_id > -1) {
    this->boxes[this->mouse_box_id]->reset();
    this->mouse_box_id = -1;
  }

  widget::Modal* modal = nullptr;
  switch (id) {
  case modal::Id::NEW_FILE_MODAL:
    modal = new (std::nothrow) widget::FileModal{};
    if (modal == nullptr) {
      logger::fatal("Could not create modal");
      std::abort();
    }

    ((widget::FileModal*)modal)
        ->init(
            {(f32)this->window.size.x, (f32)this->window.size.y},
            this->renderer, this->data
        );
    break;

  case modal::Id::NONE:
    return error_code::OK;
  }

  // NOLINTNEXTLINE
  return this->modals.push_back(std::move(modal));
}

modal::Id Manager::pop_modal() noexcept {
  assert(!this->modals.is_empty());

  auto* modal = this->modals.pop();
  auto id = modal->get_id();
  delete modal; // NOLINT

  this->reset_data();
  return id;
}

void Manager::clear_modals() noexcept {
  for (i32 i = 0; i < this->modals.get_size(); ++i) {
    delete this->modals[i]; // NOLINT
  }
  this->modals.clear();

  this->reset_data();
}

} // namespace view::sdl3

