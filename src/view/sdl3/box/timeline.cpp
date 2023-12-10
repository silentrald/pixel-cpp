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

namespace view::sdl3::widget {

inline const f32 LAYERS_NAME_WIDTH = 160.0F;
inline const f32 LAYERS_VISIBILITY_WIDTH = 40.0F;
inline const f32 LAYERS_WIDTH = LAYERS_NAME_WIDTH + LAYERS_VISIBILITY_WIDTH;
inline const f32 LINE_WIDTH = 4.0F;
inline const f32 LAYERS_NAME_PADDING_X = 4.0F;

TimelineBox::TimelineBox() noexcept
    : start_frame(0U),
      end_frame(0U),
      active_layer(0U),
      active_frame(0U),
      selected_layer(0U),
      selected_frame(0U) {}

error_code TimelineBox::init() noexcept {
  // TODO: Only temps, grow this for every add layer
  TRY(this->rects.resize(8U));
  return error_code::OK;
}

error_code TimelineBox::insert_layer_info(
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
    textbox.rect.pos = text_rect.pos;
    textbox.rect.size = {LAYERS_NAME_WIDTH, text_rect.h};
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

  // NOTE: Underflows
  for (index = index - 1U; index < USIZE_MAX; --index) {
    text_rect.y += renderer::get_text_height() + LINE_WIDTH;
    this->layers[index].textbox.rect.pos = text_rect.pos;
    this->layers[index].textbox.tex_rect.pos = {
        text_rect.pos.x + LAYERS_NAME_PADDING_X, text_rect.pos.y};
  }

  return error_code::OK;
}

void TimelineBox::set_layer_visible(usize index, bool visible) noexcept {
  this->layers[index].visible = visible;
}

void TimelineBox::set_anim(const draw::Anim* anim) noexcept {
  this->anim = anim;
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

void TimelineBox::locale_updated() noexcept {
  for (i32 i = 0; i < this->layers.get_size(); ++i) {
    this->layers[i].textbox.locale_updated();
    this->layers[i].textbox.rect.h = this->layers[i].textbox.tex_rect.h;
  }
}

void TimelineBox::input(const event::Input& evt, InputData& data) noexcept {
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
    const event::Input& evt, InputData& data
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
  // NOTE: reimplement for overflow
  if (click_rect.has_point(evt.mouse.pos)) {
    this->selected_layer =
        this->layers.get_size() -
        std::clamp(
            static_cast<usize>((evt.mouse.pos.y - click_rect.y) / size), 0U,
            this->layers.get_size()
        );
    presenter::set_selected_layer(this->selected_layer);
    presenter::open_layers_ctx_menu();
    return;
  }

  // NOTE: reimplement for overflow
  click_rect.x = this->x + LAYERS_WIDTH + LINE_WIDTH;
  click_rect.w = this->w - click_rect.x;
  if (!click_rect.has_point(evt.mouse.pos)) {
    return;
  }
  size = this->layers.front().textbox.h;
  this->selected_frame = std::clamp(
      static_cast<usize>((evt.mouse.pos.x - click_rect.x) / size), 0U,
      this->anim->get_frame_count()
  );
  presenter::set_selected_frame(this->selected_frame);
  presenter::open_timeline_ctx_menu();
}

void TimelineBox::update(f32 _delta) noexcept {
  //
}

void TimelineBox::render() noexcept {
  renderer::set_color({0x44, 0x44, 0xff, 0xff});
  renderer::fill_rect(this->rect);

  this->render_grid();
  this->render_frame_numbers();
  this->render_frames();
  this->render_layers();
}

void TimelineBox::render_grid() const noexcept {
  frect line{};

  // Selected frame
  renderer::set_color({0xff, 0xff, 0xff, 0x66});
  line.x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH +
           (this->active_frame - this->start_frame) *
               (renderer::get_text_height() + LINE_WIDTH);
  line.y = this->rect.y;
  line.w = renderer::get_text_height();
  line.h = this->rect.h;
  renderer::fill_rect(line);

  // Selected layer
  line.x = this->rect.x;
  line.y = this->rect.y + (renderer::get_text_height() + LINE_WIDTH) *
                              (this->layers.get_size() - this->active_layer);
  line.w = this->rect.w;
  line.h = renderer::get_text_height();
  renderer::fill_rect(line);

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
  for (usize i = this->start_frame; i <= this->end_frame; ++i) {
    static_cast<void>(renderer::render_number(i, off));
    off.x += renderer::get_text_height() + 4.0F; // SQUARE
  }
}

void TimelineBox::render_frames() noexcept {
  if (this->anim == nullptr || this->anim->get_layer_count() == 0U) {
    return;
  }

  renderer::set_color({0x00, 0x00, 0x00, 0xff});

  frect def_rect{.pos = this->pos};
  def_rect.w = def_rect.h = renderer::get_text_height() - 4.0F;
  def_rect.x += LAYERS_WIDTH + LINE_WIDTH + 2.0F;
  def_rect.y += renderer::get_text_height() + LINE_WIDTH + 2.0F;

  // Reverse the order
  for (usize l = this->anim->get_layer_count() - 1U; l < USIZE_MAX; --l) {
    this->rects[l].pos = def_rect.pos;
    this->rects[l].w = 0.0F;
    this->rects[l].h = def_rect.h;
    def_rect.y += renderer::get_text_height() + LINE_WIDTH;
  }

  usize id = 0U;
  auto curr_iter = this->anim->get_frame_iter();

  // Handle first frame
  for (usize l = 0; l < this->anim->get_layer_count(); ++l) {
    if (curr_iter.get_image_id(l) != 0U) {
      rects[l].w += def_rect.w;
      continue;
    }

    def_rect.pos = rects[l].pos;
    rects[l].x += def_rect.w;
    renderer::draw_rect(def_rect);
  }
  ++curr_iter;

  for (auto prev_iter = this->anim->get_frame_iter(); curr_iter;
       ++curr_iter, ++prev_iter) {
    for (usize l = 0; l < this->anim->get_layer_count(); ++l) {
      id = curr_iter.get_image_id(l);
      if (id == 0U) {
        // Empty image
        if (rects[l].w > 0.0F) {
          renderer::fill_rect(rects[l]);
        }

        def_rect.x = rects[l].x + rects[l].w + 8.0F;
        def_rect.y = rects[l].y;
        rects[l].x = def_rect.x + def_rect.w;
        rects[l].w = 0.0F;

        renderer::draw_rect(def_rect);
      } else if (id != prev_iter.get_image_id(l)) {
        // New id
        if (rects[l].w > 0.0F) {
          renderer::fill_rect(rects[l]);
        }

        rects[l].x += rects[l].w + 8.0F;
        rects[l].w = def_rect.w;
      } else {
        // Extend Prev id
        rects[l].w += renderer::get_text_height();
      }
    }
  }

  --curr_iter;
  for (usize l = 0; l < this->anim->get_layer_count(); ++l) {
    if (curr_iter.get_image_id(l) != 0U) {
      renderer::fill_rect(rects[l]);
    }
  }
}

void TimelineBox::render_layers() noexcept {
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
    this->layers[i].textbox.render();

    renderer::set_color(
        this->layers[i].visible ? rgba8{0x00, 0xff, 0x00, 0xff}
                                : rgba8{0xff, 0x00, 0x00, 0xff}
    );
    renderer::fill_rect(vis_rect);
    vis_rect.y -= height + LINE_WIDTH;
  }
}

} // namespace view::sdl3::widget
