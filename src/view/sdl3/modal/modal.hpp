/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#ifndef PXL_VIEW_SDL3_MODAL_HPP
#define PXL_VIEW_SDL3_MODAL_HPP

#include "../widget/widget.hpp"
#include "view/modal.hpp"

namespace view::sdl3::widget {

/**
 * Widget object that shows a small window message on the screen
 **/
class Modal : public virtual Widget { // NOLINT
public:
  Modal() noexcept = default;
  Modal(const Modal&) noexcept = delete;
  Modal& operator=(const Modal&) noexcept = delete;
  Modal(Modal&&) noexcept = default;
  Modal& operator=(Modal&&) noexcept = default;

  [[nodiscard]] modal::Id get_id() const noexcept {
    return this->id;
  }

  [[nodiscard]] virtual void* get_data() const noexcept = 0;

  /**
   * Call for responsive elements within the modal
   **/
  virtual void resize(const frect& rect) noexcept = 0;

  /**
   * Call when the the modal window is dragged
   **/
  virtual void drag(fvec pos) noexcept = 0;

protected:
  modal::Id id = modal::Id::NONE;

public:
  bool resizeable = true;
  bool draggable = true;
};

} // namespace view::sdl3::widget

#endif

