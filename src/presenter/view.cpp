/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-23
 *==========================*/

/**
 * Presenter functions that only updates view elements
 **/

#include "./presenter.hpp"
#include "core/logger/logger.hpp"
#include "model/model.hpp"

void presenter::set_fg_color(const std::string& hex_str) noexcept {
  model_.fg_color = color::parse_hex_string(hex_str.c_str());
  view_.set_fg_color(model_.fg_color);
}

void presenter::set_bg_color(const std::string& hex_str) noexcept {
  model_.bg_color = color::parse_hex_string(hex_str.c_str());
  view_.set_bg_color(model_.bg_color);
}

void presenter::update_view() noexcept {
  // Update the timeline
  view_.clear_layers();
  for (i32 i = model_.anim.get_layer_count() - 1; i >= 0; --i) {
    auto layer_info = model_.anim.get_layer_info(i);
    TRY_ABORT(
        view_.insert_layer(0, model_.anim.get_layer_info(i)),
        "Could not update view"
    );
  }

  view_.set_active_on_timeline(model_.frame_index, model_.layer_index);
  view_.set_frame_range(0U, model_.anim.get_frame_count() - 1U);
}

void presenter::close_modals() noexcept {
  view_.clear_modals();
}
