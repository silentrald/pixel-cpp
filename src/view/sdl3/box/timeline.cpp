/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-01
 *==========================*/

#include "./timeline.hpp"
#include "presenter/presenter.hpp"
#include "view/input.hpp"

namespace view::sdl3::widget {

inline const f32 LAYERS_NAME_WIDTH = 160.0F;
inline const f32 LAYERS_VISIBILITY_WIDTH = 40.0F;
inline const f32 LAYERS_WIDTH = LAYERS_NAME_WIDTH + LAYERS_VISIBILITY_WIDTH;
inline const f32 LINE_WIDTH = 4.0F;

void TimelineBox::init(const Renderer& renderer) noexcept {
  this->add_btn.rect.pos = this->rect.pos;
  this->add_btn.rect.size = renderer.get_text_size("Add Layer");
  this->add_btn.tex_rect = this->add_btn.rect;

  this->add_btn.set_texture(renderer.create_text("Add Layer"));
  this->add_btn.set_theme(input::BtnTheme::TOOL_BTN);
  this->add_btn.set_left_click_listener(presenter::push_back_layer);
}

void TimelineBox::insert_layer_info(
    i32 index, const draw::LayerInfo& layer_info, const Renderer& renderer
) noexcept {
  assert(index >= 0 && index <= this->layers.get_size());

  this->selected_layer = index;

  frect text_rect{};
  text_rect.x = this->rect.x + LINE_WIDTH;
  text_rect.size = renderer.get_text_size(layer_info.name);

  if (this->layers.is_empty()) {
    text_rect.y = this->rect.y + renderer.get_text_height() + LINE_WIDTH;
    this->layers.insert(
        0, {.tex = renderer.create_text(layer_info.name),
            .rect = text_rect,
            .visible = (bool)(layer_info.opacity & 0x80)}
    );
    return;
  }

  text_rect.y = this->rect.y + (renderer.get_text_height() + LINE_WIDTH) *
                                   (this->layers.get_size() - index + 1);
  this->layers.insert(
      index, {.tex = renderer.create_text(layer_info.name),
              .rect = text_rect,
              .visible = (bool)(layer_info.opacity & 0x80)}
  );

  for (index = index - 1; index >= 0; --index) {
    text_rect.y += renderer.get_text_height() + LINE_WIDTH;
    this->layers[index].rect.pos = text_rect.pos;
  }
}

void TimelineBox::set_layer_visible(i32 index, bool visible) noexcept {
  this->layers[index].visible = visible;
}

void TimelineBox::clear_layers() noexcept {
  this->layers.clear();
}

void TimelineBox::resize(const frect& rect) noexcept {
  this->rect = rect;

  auto off = this->rect.pos;
  this->add_btn.rect.pos = off;
  this->add_btn.tex_rect.pos = off;

  if (this->layers.is_empty()) {
    return;
  }

  off.x += LINE_WIDTH;
  auto height = this->layers.front().rect.h + LINE_WIDTH;
  for (i32 i = this->layers.get_size() - 1; i >= 0; --i) {
    off.y += height;
    this->layers[i].rect.pos = off;
  }
}

void TimelineBox::reset() noexcept {
  this->add_btn.reset();
}

void TimelineBox::input(const event::Input& evt, Data& data) noexcept {
  this->add_btn.input(evt, data);

  // NOTE: Only handles changing of frame/layer for now
  if (this->layers.is_empty() ||
      (evt.mouse.left.state != input::MouseState::UP)) {
    return;
  }

  f32 height = this->layers.front().rect.h;
  frect bounds{};

  // Toggle visibility
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

  // Selecting frame
  bounds.x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH;
  bounds.y = this->rect.y + height + LINE_WIDTH;
  bounds.w = this->rect.w - LAYERS_WIDTH - LINE_WIDTH;
  bounds.h = this->rect.h - height - LINE_WIDTH;
  if (!bounds.has_point(evt.mouse.pos)) {
    return;
  }

  u32 new_sel_layer = this->selected_layer;
  u32 new_sel_frame = this->selected_frame;
  bounds.size = {this->rect.w - LAYERS_WIDTH, height};

  for (u32 i = 0; i < this->layers.get_size(); ++i) {
    bounds.pos = this->layers[i].rect.pos;
    bounds.x -= LAYERS_WIDTH;
    if (bounds.has_point(evt.mouse.pos)) {
      new_sel_layer = i;
      break;
    }
  }

  bounds.pos = {this->rect.x + LAYERS_WIDTH + LINE_WIDTH, this->rect.y};
  bounds.size = {height, this->rect.h};
  for (u32 i = this->start_frame; i <= this->end_frame; ++i) {
    if (bounds.has_point(evt.mouse.pos)) {
      new_sel_frame = i;
      break;
    }
    bounds.x += LINE_WIDTH + height;
  }

  if (this->selected_layer == new_sel_layer &&
      this->selected_frame == new_sel_frame) {
    return;
  }

  presenter::set_selected(new_sel_frame, new_sel_layer);
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

  this->add_btn.render(renderer);
}

void TimelineBox::render_grid(const Renderer& renderer) const noexcept {
  frect line{};

  // Selected frame
  renderer.set_color({0xff, 0xff, 0xff, 0x66});
  line.x = this->rect.x + LAYERS_WIDTH + LINE_WIDTH +
           (this->selected_frame - this->start_frame) *
               (renderer.get_text_height() + LINE_WIDTH);
  line.y = this->rect.y;
  line.w = renderer.get_text_height();
  line.h = this->rect.h;
  renderer.fill_rect(line);

  // Selected layer
  line.x = this->rect.x;
  line.y = this->rect.y + (renderer.get_text_height() + LINE_WIDTH) *
                              (this->layers.get_size() - this->selected_layer);
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

  f32 height = this->layers.front().rect.h;
  frect vis_rect{};
  vis_rect.x = this->rect.x + LAYERS_NAME_WIDTH + 4.0F;
  vis_rect.y = this->layers.front().rect.y + 4.0F;
  vis_rect.w = LAYERS_VISIBILITY_WIDTH - 8.0F;
  vis_rect.h = height - 8.0F;

  for (i32 i = 0; i < this->layers.get_size(); ++i) {
    renderer.render_texture(this->layers[i].tex, this->layers[i].rect);

    renderer.set_color(
        this->layers[i].visible ? rgba8{0x00, 0xff, 0x00, 0xff}
                                : rgba8{0xff, 0x00, 0x00, 0xff}
    );
    renderer.fill_rect(vis_rect);
    vis_rect.y -= height + LINE_WIDTH;
  }
}

} // namespace view::sdl3::widget
