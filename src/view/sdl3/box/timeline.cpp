/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#include "./timeline.hpp"
#include "core/logger/logger.hpp"
#include "presenter/presenter.hpp"
#include "view/input.hpp"
#include <cstdint>

namespace view::sdl3::widget {

inline const f32 LAYERS_NAME_WIDTH = 160.0F;
inline const f32 LAYERS_VISIBILITY_WIDTH = 40.0F;
inline const f32 LAYERS_WIDTH = LAYERS_NAME_WIDTH + LAYERS_VISIBILITY_WIDTH;
inline const f32 LINE_WIDTH = 4.0F;
inline const f32 LAYERS_NAME_PADDING_X = 4.0F;

TimelineBox::TimelineBox() noexcept
    : start_frame(1U),
      end_frame(1U),
      active_layer(0U),
      active_frame(1U),
      selected_layer(0U) {}

void TimelineBox::init(const Renderer& renderer) noexcept {
  // TODO:
}

error_code TimelineBox::insert_layer_info(
    usize index, const draw::LayerInfo& layer_info, const Renderer& renderer
) noexcept {
  assert(index >= 0 && index <= this->layers.get_size());

  this->active_layer = index;

  frect text_rect{};
  text_rect.x = this->rect.x;
  text_rect.size = renderer.get_text_size(layer_info.name);

  Textbox textbox{};
  textbox.set_text(layer_info.name, renderer);

  if (this->layers.is_empty()) {
    text_rect.y = this->rect.y + renderer.get_text_height() + LINE_WIDTH;
    textbox.rect.pos = text_rect.pos;
    textbox.rect.size = {LAYERS_NAME_WIDTH, text_rect.h};
    textbox.tex_rect.pos = {text_rect.x + LAYERS_NAME_PADDING_X, text_rect.y};
    textbox.tex_rect.size = text_rect.size;

    return this->layers.insert(
        0, {.textbox = std::move(textbox),
            .visible = (bool)(layer_info.opacity & 0x80)}
    );
  }

  text_rect.y = this->rect.y + (renderer.get_text_height() + LINE_WIDTH) *
                                   (this->layers.get_size() - index + 1);
  textbox.rect.pos = text_rect.pos;
  textbox.rect.size = {LAYERS_NAME_WIDTH, text_rect.h};
  textbox.tex_rect.pos = {text_rect.x + LAYERS_NAME_PADDING_X, text_rect.y};
  textbox.tex_rect.size = text_rect.size;

  TRY(this->layers.insert(
      index, {.textbox = std::move(textbox),
              .visible = (bool)(layer_info.opacity & 0x80)}
  ));

  // NOTE: Underflows
  for (index = index - 1U; index < UINT32_MAX; --index) {
    text_rect.y += renderer.get_text_height() + LINE_WIDTH;
    this->layers[index].textbox.rect.pos = text_rect.pos;
    this->layers[index].textbox.tex_rect.pos = {
        text_rect.pos.x + LAYERS_NAME_PADDING_X, text_rect.pos.y};
  }

  return error_code::OK;
}

void TimelineBox::set_layer_visible(usize index, bool visible) noexcept {
  this->layers[index].visible = visible;
}

void TimelineBox::clear_layers() noexcept {
  this->layers.clear();
}

void TimelineBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  auto off = this->rect.pos;

  if (this->layers.is_empty()) {
    return;
  }

  auto height = this->layers.front().textbox.rect.h + LINE_WIDTH;
  for (i32 i = this->layers.get_size() - 1; i >= 0; --i) {
    off.y += height;
    this->layers[i].textbox.rect.pos = off;
    this->layers[i].textbox.tex_rect.pos = {
        off.x + LAYERS_NAME_PADDING_X, off.y};
  }
}

void TimelineBox::reset() noexcept {
  // TODO:
}

void TimelineBox::locale_updated(const Renderer& renderer) noexcept {
  for (i32 i = 0; i < this->layers.get_size(); ++i) {
    this->layers[i].textbox.locale_updated(renderer);
    this->layers[i].textbox.rect.h = this->layers[i].textbox.tex_rect.h;
  }
}

void TimelineBox::input(const event::Input& evt, Data& data) noexcept {
  // NOTE: Only handles changing of frame/layer for now
  if (this->layers.is_empty()) {
    return;
  }

  if (evt.mouse.left != input::MouseState::NONE) {
    this->handle_mouse_left(evt, data);
  } else if (evt.mouse.right != input::MouseState::NONE) {
    this->handle_mouse_right(evt, data);
  }
}

void TimelineBox::handle_mouse_left(
    const event::Input& evt, Data& data
) noexcept {
  f32 height = this->layers.front().textbox.rect.h;
  frect bounds{};

  // Toggle visibility
  if (evt.mouse.left == input::MouseState::UP) {
    bounds.x = this->rect.x + LAYERS_NAME_WIDTH + 4.0F;
    bounds.y =
        this->rect.y + 4.0F + (height + LINE_WIDTH) * this->layers.get_size();
    bounds.w = LAYERS_VISIBILITY_WIDTH - 8.0F;
    bounds.h = height - 8.0F;

    for (i32 i = 0; i < this->layers.get_size(); ++i) {
      if (bounds.has_point(evt.mouse.pos)) {
        presenter::toggle_visibility(i);
        return;
      }
      bounds.y -= height + LINE_WIDTH;
    }
  }

  // Selecting frame
  bounds.x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH;
  bounds.y = this->rect.y + height + LINE_WIDTH;
  bounds.w = this->rect.w - LAYERS_WIDTH - LINE_WIDTH;
  bounds.h = this->rect.h - height - LINE_WIDTH;
  if (!bounds.has_point(evt.mouse.pos)) {
    return;
  }

  usize new_active_layer = this->active_layer;
  usize new_active_frame = this->active_frame;
  bounds.size = {this->rect.w - LAYERS_WIDTH, height};

  for (usize i = 0; i < this->layers.get_size(); ++i) {
    bounds.pos = this->layers[i].textbox.rect.pos;
    bounds.x -= LAYERS_WIDTH;
    if (bounds.has_point(evt.mouse.pos)) {
      new_active_layer = i;
      break;
    }
  }

  bounds.pos = {this->rect.x + LAYERS_WIDTH + LINE_WIDTH, this->rect.y};
  bounds.size = {height, this->rect.h};
  for (usize i = this->start_frame; i <= this->end_frame; ++i) {
    if (bounds.has_point(evt.mouse.pos)) {
      new_active_frame = i;
      break;
    }
    bounds.x += LINE_WIDTH + height;
  }

  if (this->active_layer == new_active_layer &&
      this->active_frame == new_active_frame) {
    return;
  }

  if (evt.mouse.left == input::MouseState::UP) {
    presenter::set_active_image(new_active_frame, new_active_layer);
  } else {
    this->active_frame = new_active_frame;
    this->active_layer = new_active_layer;
  }
}

void TimelineBox::handle_mouse_right(
    const event::Input& evt, Data& data
) noexcept {
  if (evt.mouse.right != input::MouseState::UP) {
    return;
  }

  frect click_rect{};
  auto height = this->layers.front().textbox.h + LINE_WIDTH;

  click_rect.x = this->x;
  click_rect.y = this->y + height;
  click_rect.w = LAYERS_WIDTH;
  click_rect.h = this->h - height;
  // NOTE: reimplement for overflow
  if (click_rect.has_point(evt.mouse.pos)) {
    this->selected_layer =
        this->layers.get_size() -
        std::clamp(
            (usize)((evt.mouse.pos.y - click_rect.y) / height), 0U,
            this->layers.get_size()
        );
    presenter::set_selected_layer(this->selected_layer);
    presenter::open_layers_ctx_menu();
    return;
  }

  // NOTE: reimplement for overflow
  click_rect.x = LAYERS_WIDTH + LINE_WIDTH;
  click_rect.w = this->w - click_rect.x;
  if (!click_rect.has_point(evt.mouse.pos)) {
    return;
  }
  this->selected_layer = this->layers.get_size() -
                         std::clamp(
                             (usize)((evt.mouse.pos.y - click_rect.y) / height),
                             0U, this->layers.get_size()
                         );
  presenter::set_selected_layer(this->selected_layer);
  presenter::open_timeline_ctx_menu();
}

void TimelineBox::update() noexcept {
  //
}

void TimelineBox::render(const Renderer& renderer) const noexcept {
  renderer.set_color({0x44, 0x44, 0xff, 0xff});
  renderer.fill_rect(this->rect);

  this->render_grid(renderer);
  this->render_frames(renderer);
  this->render_layers(renderer);
}

void TimelineBox::render_grid(const Renderer& renderer) const noexcept {
  frect line{};

  // Selected frame
  renderer.set_color({0xff, 0xff, 0xff, 0x66});
  line.x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH +
           (this->active_frame - this->start_frame) *
               (renderer.get_text_height() + LINE_WIDTH);
  line.y = this->rect.y;
  line.w = renderer.get_text_height();
  line.h = this->rect.h;
  renderer.fill_rect(line);

  // Selected layer
  line.x = this->rect.x;
  line.y = this->rect.y + (renderer.get_text_height() + LINE_WIDTH) *
                              (this->layers.get_size() - this->active_layer);
  line.w = this->rect.w;
  line.h = renderer.get_text_height();
  renderer.fill_rect(line);

  // Horizontal lines
  renderer.set_color({0x00, 0x00, 0x00, 0xff});
  line.x = this->rect.x;
  line.y = this->rect.y + renderer.get_text_height();
  line.w = this->rect.w;
  line.h = LINE_WIDTH;
  renderer.fill_rect(line);

  // Vertical lines
  line.x = this->rect.x + LAYERS_WIDTH;
  line.y = this->rect.y;
  line.w = LINE_WIDTH;
  line.h = this->rect.h;
  renderer.fill_rect(line);
}

void TimelineBox::render_frames(const Renderer& renderer) const noexcept {
  fvec off{
      .x =
          this->rect.x + LAYERS_WIDTH + LINE_WIDTH + renderer.get_text_height(),
      .y = this->rect.y};
  for (i32 i = this->start_frame; i <= this->end_frame; ++i) {
    (void)renderer.render_number(i, off);
    off.x += renderer.get_text_height() + LINE_WIDTH; // SQUARE
  }
}

void TimelineBox::render_layers(const Renderer& renderer) const noexcept {
  if (this->layers.is_empty()) {
    return;
  }

  f32 height = this->layers.front().textbox.rect.h;
  frect vis_rect{};
  vis_rect.x = this->rect.x + LAYERS_NAME_WIDTH + 4.0F;
  vis_rect.y = this->layers.front().textbox.rect.y + 4.0F;
  vis_rect.w = LAYERS_VISIBILITY_WIDTH - 8.0F;
  vis_rect.h = height - 8.0F;

  for (i32 i = 0; i < this->layers.get_size(); ++i) {
    this->layers[i].textbox.render(renderer);

    renderer.set_color(
        this->layers[i].visible ? rgba8{0x00, 0xff, 0x00, 0xff}
                                : rgba8{0xff, 0x00, 0x00, 0xff}
    );
    renderer.fill_rect(vis_rect);
    vis_rect.y -= height + LINE_WIDTH;
  }
}

} // namespace view::sdl3::widget
