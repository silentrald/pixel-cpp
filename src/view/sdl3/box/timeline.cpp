/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#include "./timeline.hpp"
#include "core/logger/logger.hpp"
#include "math.hpp"
#include "presenter/presenter.hpp"
#include "view/input.hpp"
#include <algorithm>
#include <cmath>

namespace view::sdl3::widget {

inline const f32 LAYERS_NAME_WIDTH = 160.0F;
inline const f32 LAYERS_VISIBILITY_WIDTH = 40.0F;
inline const f32 LAYERS_WIDTH = LAYERS_NAME_WIDTH + LAYERS_VISIBILITY_WIDTH;
inline const f32 LINE_WIDTH = 4.0F;
inline const f32 LAYERS_NAME_PADDING_X = 4.0F;
inline const u32 LAYER_ADJUSTMENT = 1U;

inline const f32 SCROLL_WIDTH = 8.0F;

TimelineBox::TimelineBox() noexcept
    : layer_offset(0U),
      max_layers(0U),
      frame_offset(0U),
      max_frames(0U),
      active_layer(0U),
      active_frame(0U),
      selected_layer(0U),
      selected_frame(0U) {}

void TimelineBox::init() noexcept {
  this->vertical_scrollbar.set_x(this->x + this->w - SCROLL_WIDTH);
  this->vertical_scrollbar.set_y(
      this->y + renderer::get_text_height() + LINE_WIDTH
  );
  this->vertical_scrollbar.set_width(SCROLL_WIDTH);
  this->vertical_scrollbar.set_height(
      this->h - renderer::get_text_height() - LINE_WIDTH - SCROLL_WIDTH
  );

  this->horizontal_scrollbar.set_x(this->x + LAYERS_WIDTH + LINE_WIDTH);
  this->horizontal_scrollbar.set_y(this->y + this->h - SCROLL_WIDTH);
  this->horizontal_scrollbar.set_width(
      this->w - LAYERS_WIDTH - LINE_WIDTH - SCROLL_WIDTH
  );
  this->horizontal_scrollbar.set_height(SCROLL_WIDTH);
}

// === Setters === //

void TimelineBox::set_layer_offset(usize layer_offset) noexcept {
  assert(this->anim != nullptr);
  assert(layer_offset < this->anim->get_layer_count());

  this->layer_offset = layer_offset;
  f32 diff = this->anim->get_layer_count() - this->max_layers;
  this->vertical_scrollbar.set_percentage(1.0F - (this->layer_offset / diff));

  this->adjust_layer_textboxes();
}

void TimelineBox::adjust_layer_textboxes() noexcept {
  auto height = renderer::get_text_height() + LINE_WIDTH;
  auto end =
      std::min(layer_offset + this->max_layers, this->anim->get_layer_count());

  fvec offset{};
  offset.x = this->x;
  offset.y = this->y + height * (end - this->layer_offset);

  for (usize i = this->layer_offset; i < end; ++i) {
    this->layers[i].textbox.pos = offset;
    this->layers[i].textbox.tex_rect.x = offset.x + LAYERS_NAME_PADDING_X;
    this->layers[i].textbox.tex_rect.y = offset.y;

    offset.y -= height;
  }
}

void TimelineBox::set_frame_offset(usize frame_offset) noexcept {
  assert(this->anim != nullptr);
  assert(frame_offset < this->anim->get_frame_count());

  this->frame_offset = frame_offset;
  this->horizontal_scrollbar.set_percentage(
      this->frame_offset /
      (this->anim->get_frame_count() - this->max_frames + 1.0F)
  );
}

void TimelineBox::set_active_image(
    usize frame_index, usize layer_index
) noexcept {
  assert(this->anim != nullptr);
  assert(frame_index < this->anim->get_frame_count());
  assert(layer_index < this->anim->get_layer_count());

  this->active_frame = frame_index;
  this->active_layer = layer_index;

  if (this->active_layer - LAYER_ADJUSTMENT <= this->layer_offset) {
    this->set_layer_offset(this->active_layer - LAYER_ADJUSTMENT);
  } else if (this->active_layer >= this->layer_offset + this->max_layers) {
    this->set_layer_offset(this->active_layer - this->max_layers + 1U);
  }

  if (this->active_frame < this->frame_offset) {
    this->set_frame_offset(this->active_frame);
  } else if (this->active_frame >= this->frame_offset + this->max_frames - 1U) {
    this->set_frame_offset(this->active_frame - this->max_frames + 2U);
  }
}

error_code TimelineBox::set_anim(const draw::Anim* anim) noexcept {
  assert(anim != nullptr);

  this->anim = anim;
  TRY(this->rects.resize(anim->get_layer_count()));

  this->max_layers = std::floor(
      (this->h - renderer::get_text_height() - LINE_WIDTH - SCROLL_WIDTH) /
      (renderer::get_text_height() + LINE_WIDTH)
  );
  this->max_frames = std::ceil(
      (this->w - LAYERS_WIDTH - LINE_WIDTH - SCROLL_WIDTH) /
      (renderer::get_text_height() + LINE_WIDTH)
  );
  this->vertical_scrollbar.set_real_height(
      anim->get_layer_count() * renderer::get_text_height()
  );
  this->horizontal_scrollbar.set_real_width(
      anim->get_frame_count() * (renderer::get_text_height() + LINE_WIDTH)
  );

  return error_code::OK;
}

// === Modifiers === //

error_code TimelineBox::insert_layer(
    usize index, const draw::LayerInfo& layer_info
) noexcept {
  assert(index >= 0 && index <= this->layers.get_size());

  this->active_layer = index;

  frect text_rect{};
  text_rect.x = this->rect.x;
  text_rect.size = renderer::get_text_size(layer_info.name);

  Textbox textbox{};
  textbox.set_text(layer_info.name);

  if (this->layers.is_empty()) {
    text_rect.y = this->rect.y + renderer::get_text_height() + LINE_WIDTH;
    textbox.pos = text_rect.pos;
    textbox.size = {LAYERS_NAME_WIDTH, text_rect.h};
    textbox.tex_rect.pos = {text_rect.x + LAYERS_NAME_PADDING_X, text_rect.y};
    textbox.tex_rect.size = text_rect.size;

    return this->layers.insert(
        0, {.textbox = std::move(textbox),
            .visible = (bool)(layer_info.opacity & 0x80)}
    );
  }

  text_rect.y = this->rect.y + (renderer::get_text_height() + LINE_WIDTH) *
                                   (this->layers.get_size() - index + 1);
  textbox.rect.pos = text_rect.pos;
  textbox.rect.size = {LAYERS_NAME_WIDTH, text_rect.h};
  textbox.tex_rect.pos = {text_rect.x + LAYERS_NAME_PADDING_X, text_rect.y};
  textbox.tex_rect.size = text_rect.size;

  TRY(this->layers.insert(
      index, {.textbox = std::move(textbox),
              .visible = (bool)(layer_info.opacity & 0x80)}
  ));

  TRY(this->rects.push_back({}));

  for (index = index - 1U; index < USIZE_MAX; --index) {
    text_rect.y += renderer::get_text_height() + LINE_WIDTH;
    this->layers[index].textbox.pos = text_rect.pos;
    this->layers[index].textbox.tex_rect.pos = {
        text_rect.pos.x + LAYERS_NAME_PADDING_X, text_rect.pos.y};
  }

  this->vertical_scrollbar.set_real_height(
      anim->get_layer_count() * renderer::get_text_height()
  );

  return error_code::OK;
}

void TimelineBox::remove_layer(usize index) noexcept {
  fvec off = this->layers[index].textbox.pos;
  this->layers.remove(index);

  for (index = index - 1U; index < USIZE_MAX; --index) {
    this->layers[index].textbox.pos = off;
    this->layers[index].textbox.tex_rect.pos = {
        off.x + LAYERS_NAME_PADDING_X, off.y};
    off.y += renderer::get_text_height() + LINE_WIDTH;
  }

  this->rects.pop();
}

void TimelineBox::clear_layers() noexcept {
  this->layers.clear();
}

void TimelineBox::set_layer_visible(usize index, bool visible) noexcept {
  this->layers[index].visible = visible;
}

void TimelineBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  auto off = this->rect.pos;

  if (this->layers.is_empty()) {
    return;
  }

  auto height = renderer::get_text_height() + LINE_WIDTH;
  for (i32 i = this->layers.get_size() - 1; i >= 0; --i) {
    off.y += height;
    this->layers[i].textbox.rect.pos = off;
    this->layers[i].textbox.tex_rect.pos = {
        off.x + LAYERS_NAME_PADDING_X, off.y};
  }

  this->max_layers = std::floor(
      (this->h - renderer::get_text_height() - LINE_WIDTH - SCROLL_WIDTH) /
      (renderer::get_text_height() + LINE_WIDTH)
  );
  this->max_frames = std::ceil(
      (this->w - LAYERS_WIDTH - LINE_WIDTH) /
      (renderer::get_text_height() + LINE_WIDTH)
  );

  this->vertical_scrollbar.set_x(this->x + this->w - SCROLL_WIDTH);
  this->vertical_scrollbar.set_y(
      this->y + renderer::get_text_height() + LINE_WIDTH
  );
  this->vertical_scrollbar.set_height(
      this->h - renderer::get_text_height() - LINE_WIDTH - SCROLL_WIDTH
  );
  this->adjust_layer_textboxes();

  this->horizontal_scrollbar.set_x(this->x + LAYERS_WIDTH + LINE_WIDTH);
  this->horizontal_scrollbar.set_y(this->y + this->h - SCROLL_WIDTH);
  this->horizontal_scrollbar.set_width(
      this->w - LAYERS_WIDTH - LINE_WIDTH - SCROLL_WIDTH
  );
}

void TimelineBox::reset() noexcept {
  // TODO:
}

void TimelineBox::update_locale() noexcept {
  for (i32 i = 0; i < this->layers.get_size(); ++i) {
    this->layers[i].textbox.update_locale();
    this->layers[i].textbox.rect.h = this->layers[i].textbox.tex_rect.h;
  }
}

void TimelineBox::input(const event::Input& evt, InputData& data) noexcept {
  // NOTE: Only handles changing of frame/layer for now
  if (this->anim == nullptr || this->layers.is_empty()) {
    return;
  }

  if (evt.mouse.wheel.y > 0.0F) {
    if (this->layer_offset < this->anim->get_layer_count() - this->max_layers) {
      this->set_layer_offset(this->layer_offset + 1U);
    }
  } else if (evt.mouse.wheel.y < 0.0F) {
    if (this->layer_offset > 0) {
      this->set_layer_offset(this->layer_offset - 1U);
    }
  }

  if (evt.mouse.wheel.x > 0.0F) {
    if (this->frame_offset <=
        this->anim->get_frame_count() - this->max_frames) {
      this->set_frame_offset(this->frame_offset + 1U);
    }
  } else if (evt.mouse.wheel.x < 0.0F) {
    if (this->frame_offset > 0) {
      this->set_frame_offset(this->frame_offset - 1U);
    }
  }

  if (evt.mouse.left != input::MouseState::NONE) {
    this->vertical_scrollbar.input(evt, data);
    usize new_offset = (this->anim->get_layer_count() - this->max_layers) *
                       (1.0F - this->vertical_scrollbar.get_percentage());
    if (new_offset != this->layer_offset) {
      this->layer_offset = new_offset;
      this->adjust_layer_textboxes();
    }
    if (&this->vertical_scrollbar == data.left_clicked_widget) {
      return;
    }

    this->horizontal_scrollbar.input(evt, data);
    new_offset = (this->anim->get_frame_count() - this->max_frames + 1U) *
                 this->horizontal_scrollbar.get_percentage();
    if (new_offset != this->frame_offset) {
      this->frame_offset = new_offset;
      this->adjust_layer_textboxes();
    }
    if (&this->horizontal_scrollbar == data.left_clicked_widget) {
      return;
    }

    this->handle_mouse_left(evt, data);
  } else if (evt.mouse.right != input::MouseState::NONE) {
    this->handle_mouse_right(evt, data);
  }
}

void TimelineBox::handle_mouse_left(
    const event::Input& evt, InputData& data
) noexcept {
  f32 height = renderer::get_text_height() + LINE_WIDTH;
  frect bounds{};

  // Toggle visibility
  if (evt.mouse.left == input::MouseState::UP) {
    bounds.x = this->rect.x + LAYERS_NAME_WIDTH + 4.0F;
    bounds.y = this->rect.y + renderer::get_text_height() + LINE_WIDTH;
    bounds.w = LAYERS_VISIBILITY_WIDTH - 8.0F;
    bounds.h = (renderer::get_text_height() + LINE_WIDTH) * this->max_layers;

    if (bounds.has_point(evt.mouse.pos)) {
      auto layer = this->max_layers -
                              (usize)((evt.mouse.pos.y - bounds.y) / height) +
                              this->layer_offset - 1U;
      presenter::toggle_visibility(layer);
      return;
    }
  }

  // Selecting an image
  bounds.x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH;
  bounds.y = this->rect.y + height;
  bounds.w = this->rect.w - LAYERS_WIDTH - LINE_WIDTH;
  bounds.h = this->max_layers * height;
  if (!bounds.has_point(evt.mouse.pos)) {
    return;
  }

  auto new_active_layer = this->max_layers -
                          (usize)((evt.mouse.pos.y - bounds.y) / height) +
                          this->layer_offset - 1U;
  auto new_active_frame =
      this->frame_offset + (usize)((evt.mouse.pos.x - bounds.x) / height);

  if (this->active_layer == new_active_layer &&
      this->active_frame == new_active_frame) {
    return;
  }

  if (evt.mouse.left == input::MouseState::UP) {
    presenter::set_active_image(new_active_frame, new_active_layer);
  }
}

void TimelineBox::handle_mouse_right(
    const event::Input& evt, InputData& data
) noexcept {
  if (evt.mouse.right != input::MouseState::UP) {
    return;
  }

  frect click_rect{};
  auto size = this->layers.front().textbox.h + LINE_WIDTH;

  click_rect.x = this->x;
  click_rect.y = this->y + size;
  click_rect.w = LAYERS_WIDTH;
  click_rect.h = this->h - size;
  if (click_rect.has_point(evt.mouse.pos)) {
    this->selected_layer =
        this->layers.get_size() -
        std::clamp(
            static_cast<usize>((evt.mouse.pos.y - click_rect.y) / size),
            USIZE_ZERO, this->layers.get_size()
        ) -
        1U;
    presenter::set_selected_layer(this->selected_layer);
    presenter::open_layers_ctx_menu();
    return;
  }

  click_rect.x = this->x + LAYERS_WIDTH + LINE_WIDTH;
  click_rect.w = this->w - click_rect.x;
  if (!click_rect.has_point(evt.mouse.pos)) {
    return;
  }
  size = this->layers.front().textbox.h;
  this->selected_frame = std::clamp(
      static_cast<usize>((evt.mouse.pos.x - click_rect.x) / size), USIZE_ZERO,
      this->anim->get_frame_count()
  );
  presenter::set_selected_frame(this->selected_frame);
  presenter::open_timeline_ctx_menu();
}

void TimelineBox::update(f32 delta) noexcept {
  //
}

void TimelineBox::render() noexcept {
  renderer::set_color({0x44, 0x44, 0xff, 0xff});
  renderer::fill_rect(this->rect);

  this->render_grid();
  if (this->anim == nullptr) {
    return;
  }

  this->render_frame_numbers();
  this->render_frames();
  this->render_layers();

  // Scrolls
  this->vertical_scrollbar.render();
  this->horizontal_scrollbar.render();

  renderer::set_color({0x22, 0x22, 0x22, 0xff});
  renderer::fill_rect(
      {this->x + this->w - SCROLL_WIDTH, this->y + this->h - SCROLL_WIDTH,
       SCROLL_WIDTH, SCROLL_WIDTH}
  );
}

void TimelineBox::render_grid() const noexcept {
  frect line{};
  renderer::set_color({0xff, 0xff, 0xff, 0x66});

  // Selected frame
  if (this->active_frame >= this->frame_offset &&
      this->active_frame < this->frame_offset + this->max_frames) {
    line.x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH +
             (this->active_frame - this->frame_offset) *
                 (renderer::get_text_height() + LINE_WIDTH);
    line.y = this->rect.y;
    line.w = renderer::get_text_height();
    line.h = this->rect.h;
    renderer::fill_rect(line);
  }

  // Selected layer
  if (this->active_layer >= this->layer_offset &&
      this->active_layer < this->layer_offset + this->max_layers) {
    line.x = this->rect.x;
    line.y = this->rect.y +
             (renderer::get_text_height() + LINE_WIDTH) *
                 (this->max_layers - this->active_layer + this->layer_offset);
    line.w = this->rect.w;
    line.h = renderer::get_text_height();
    renderer::fill_rect(line);
  }

  // Horizontal lines
  renderer::set_color({0x00, 0x00, 0x00, 0xff});
  line.x = this->rect.x;
  line.y = this->rect.y + renderer::get_text_height();
  line.w = this->rect.w;
  line.h = LINE_WIDTH;
  renderer::fill_rect(line);

  // Vertical lines
  line.x = this->rect.x + LAYERS_WIDTH;
  line.y = this->rect.y;
  line.w = LINE_WIDTH;
  line.h = this->rect.h;
  renderer::fill_rect(line);
}

void TimelineBox::render_frame_numbers() const noexcept {
  fvec off{
      .x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH +
           renderer::get_text_height(),
      .y = this->rect.y};
  auto end = std::min(
      this->frame_offset + this->max_frames, this->anim->get_frame_count()
  );
  for (usize i = this->frame_offset; i < end; ++i) {
    static_cast<void>(renderer::render_number(i, off));
    off.x += renderer::get_text_height() + LINE_WIDTH; // SQUARE
  }
}

void TimelineBox::render_frames() noexcept {
  if (this->anim->get_layer_count() == 0U) {
    return;
  }

  renderer::set_color({0x00, 0x00, 0x00, 0xff});

  frect def_rect{.pos = this->pos};
  def_rect.w = def_rect.h = renderer::get_text_height() - 4.0F;
  def_rect.x += LAYERS_WIDTH + LINE_WIDTH + 2.0F;
  def_rect.y += renderer::get_text_height() + LINE_WIDTH + 2.0F;

  // Reverse the order
  for (usize r = this->max_layers - 1U; r < USIZE_MAX; --r) {
    this->rects[r].pos = def_rect.pos;
    this->rects[r].w = 0.0F;
    this->rects[r].h = def_rect.h;
    def_rect.y += renderer::get_text_height() + LINE_WIDTH;
  }

  usize id = 0U;
  auto curr_iter = this->anim->get_frame_iter(this->frame_offset);
  auto layer_end = std::min(
      this->layer_offset + this->max_layers, this->anim->get_layer_count()
  );

  // Handle first frame
  for (usize r = 0U, l = this->layer_offset; l < layer_end; ++r, ++l) {
    if (curr_iter.get_image_id(l) != 0U) {
      rects[r].w += def_rect.w;
      continue;
    }

    def_rect.pos = rects[r].pos;
    rects[r].x += def_rect.w;
    renderer::draw_rect(def_rect);
  }
  ++curr_iter;

  for (auto prev_iter = this->anim->get_frame_iter(this->frame_offset);
       curr_iter; ++curr_iter, ++prev_iter) {
    for (usize r = 0, l = this->layer_offset; l < layer_end; ++r, ++l) {
      id = curr_iter.get_image_id(l);
      if (id == 0U) {
        // Empty image
        if (rects[r].w > 0.0F) {
          renderer::fill_rect(rects[r]);
        }

        def_rect.x = rects[r].x + rects[r].w + 8.0F;
        def_rect.y = rects[r].y;
        rects[r].x = def_rect.x + def_rect.w;
        rects[r].w = 0.0F;

        renderer::draw_rect(def_rect);
      } else if (id != prev_iter.get_image_id(l)) {
        // New id
        if (rects[r].w > 0.0F) {
          renderer::fill_rect(rects[r]);
        }

        rects[r].x += rects[r].w + 8.0F;
        rects[r].w = def_rect.w;
      } else {
        // Extend Prev id
        rects[r].w += renderer::get_text_height();
      }
    }
  }

  --curr_iter;
  for (usize r = 0, l = this->layer_offset; l < layer_end; ++r, ++l) {
    if (curr_iter.get_image_id(l) != 0U) {
      renderer::fill_rect(rects[r]);
    }
  }
}

void TimelineBox::render_layers() noexcept {
  if (this->layers.is_empty()) {
    return;
  }

  frect vis_rect{};
  vis_rect.x = this->rect.x + LAYERS_NAME_WIDTH + 4.0F;
  vis_rect.y = this->layers[this->layer_offset].textbox.y + 4.0F;
  vis_rect.w = LAYERS_VISIBILITY_WIDTH - 8.0F;
  vis_rect.h = renderer::get_text_height() - 8.0F;

  auto end = std::min(
      this->anim->get_layer_count(), this->layer_offset + this->max_layers
  );
  for (usize i = this->layer_offset; i < end; ++i) {
    this->layers[i].textbox.render();

    renderer::set_color(
        this->layers[i].visible ? rgba8{0x00, 0xff, 0x00, 0xff}
                                : rgba8{0xff, 0x00, 0x00, 0xff}
    );
    renderer::fill_rect(vis_rect);
    vis_rect.y -= renderer::get_text_height() + LINE_WIDTH;
  }
}

} // namespace view::sdl3::widget
