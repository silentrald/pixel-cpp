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
  this->layers.init(size.x * size.y * get_color_type_size(this->type), 64);
  this->frames.init();

  this->layers_id = 1U;
}

void Anim::copy(const Anim& other) noexcept {
  if (this == &other) {
    return;
  }

  this->layers.copy(other.layers);
  this->frames.copy(other.frames);

  this->layers_id = other.layers_id;
  this->size = other.size;
  this->type = other.type;
}

void Anim::minicopy(const Anim& other) noexcept {
  if (this == &other) {
    return;
  }

  this->layers.minicopy(other.layers);
  this->frames.copy(other.frames);

  this->layers_id = other.layers_id;
  this->size = other.size;
  this->type = other.type;
}

void Anim::clear() noexcept {
  // TODO: check if other data is needed to be stored
}

// === Getters === //

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
  return this->frames.get_layer_count();
}

// === Modifiers === //
u32 Anim::insert_layer(i32 index) noexcept {
  assert(index >= 0 && index <= this->frames.get_layer_count());

  // Create a new layer in the layers data
  auto id = ++this->layers_id;
  this->layers.create_layer(id);

  // Insert it into the first frame
  this->frames.insert_layer(index, id);

  return id;
}

Layer Anim::get_layer(u32 id) const noexcept {
  return Layer(this->layers.get_layer(id), this->size, this->type, id);
}

u32 Anim::get_layer_id(i32 frame_id, i32 layer_index) const noexcept {
  return this->frames.get_layer_id(frame_id, layer_index);
}

Anim::operator bool() const noexcept {
  return this->layers.get_ptr() != nullptr;
}

} // namespace draw

