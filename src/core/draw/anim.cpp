/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-29
 *==========================*/

#include "./anim.hpp"
#include "core/logger/logger.hpp"
#include "math.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>

namespace draw {

inline const i32 MAX_LENGTH = 1024;

void Anim::init(ivec size, ColorType type) noexcept {
  // NOTE: Size cap can be bigger if datatypes are changed to 64 bit datatypes
  //   but MAX_LENGTH is sufficient enough
  if (size.x < 0 || size.y < 0 || size.x > MAX_LENGTH || size.y > MAX_LENGTH) {
    logger::fatal("Size (%d, %d) is either to big or negative", size.x, size.y);
    std::abort();
  }

  this->size = size;
  this->type = type;

  // TODO: Compute whether layers_max_bytes or layers_capacity is set
  this->images.init(size.x * size.y * get_color_type_size(this->type), 64);
  this->timeline.init();

  this->images_id = 1U;
}

void Anim::copy(const Anim& other) noexcept {
  if (this == &other) {
    return;
  }

  this->images.copy(other.images);
  this->timeline.copy(other.timeline);

  this->images_id = other.images_id;
  this->size = other.size;
  this->type = other.type;
}

void Anim::minicopy(const Anim& other) noexcept {
  if (this == &other) {
    return;
  }

  this->images.minicopy(other.images);
  this->timeline.copy(other.timeline);

  this->images_id = other.images_id;
  this->size = other.size;
  this->type = other.type;
}

void Anim::clear() noexcept {
  // TODO: check if other data is needed to be stored
}

// === Getters === //

const ImageDb& Anim::get_image_db() const noexcept {
  return this->images;
}

const TimelineInfo& Anim::get_timeline_info() const noexcept {
  return this->timeline;
}

bool Anim::has_point(ivec pos) const noexcept {
  return pos.x >= 0 && pos.y >= 0 && pos.x < this->size.x &&
         pos.y < this->size.y;
}

ivec Anim::get_size() const noexcept {
  return this->size;
}

i32 Anim::get_width() const noexcept {
  return this->size.x;
}

i32 Anim::get_height() const noexcept {
  return this->size.y;
}

i32 Anim::get_layer_count() const noexcept {
  return this->timeline.get_layer_count();
}

i32 Anim::get_frame_count() const noexcept {
  return this->timeline.get_frame_count();
}

Image Anim::get_image(u32 id) const noexcept {
  return Image{this->images.get_image(id), this->size, this->type, id};
}

Image Anim::get_image(u32 frame_id, i32 layer_index) const noexcept {
  auto id = this->get_image_id(frame_id, layer_index);
  return Image{this->images.get_image(id), this->size, this->type, id};
}

u32 Anim::get_image_id(u32 frame_id, i32 layer_index) const noexcept {
  return this->timeline.get_image_id(frame_id, layer_index);
}

const LayerInfo& Anim::get_layer_info(i32 index) const noexcept {
  return this->timeline.get_layer_info(index);
}

const c8* Anim::get_layer_name(i32 index) const noexcept {
  return this->timeline.get_layer_name(index);
}

bool Anim::is_layer_visible(i32 index) const noexcept {
  return this->timeline.is_layer_visible(index);
}

void Anim::get_flatten(
    u32 frame_id, i32 start_layer, i32 end_layer, ds::vector<rgba8>& pixels
) const noexcept {
  assert(frame_id != 0U);
  assert(pixels.get_size() == this->size.x * this->size.y);
  assert(start_layer <= end_layer);
  assert(start_layer >= 0 && start_layer < this->timeline.get_layer_count());
  assert(end_layer >= 0 && end_layer < this->timeline.get_layer_count());

  rgba8* img_ptr = nullptr;
  auto frame = this->timeline.get_frame(frame_id);
  for (i32 i = start_layer; i <= end_layer; ++i) {
    if (!this->timeline.is_layer_visible(i)) {
      continue;
    }

    img_ptr = (rgba8*)this->images.get_image(frame.get_image_id(i));
    // NOTE: Add blending calculations here
    for (i32 i = 0; i < pixels.get_size(); ++i) {
      if (img_ptr[i].a) {
        pixels[i] = img_ptr[i];
      }
    }
  }
}

Anim::operator bool() const noexcept {
  return this->images.get_ptr() != nullptr;
}

// === Modifiers === //

u32 Anim::insert_layer(i32 index) noexcept {
  assert(index >= 0 && index <= this->timeline.get_layer_count());

  // Create a new layer in the layers data
  auto id = ++this->images_id;
  this->images.create_layer(id);

  // Insert it into the first frame
  this->timeline.insert_layer(index, id);

  return id;
}

bool Anim::toggle_layer_visibility(i32 layer_index) noexcept {
  return this->timeline.toggle_layer_visibility(layer_index);
}

} // namespace draw

