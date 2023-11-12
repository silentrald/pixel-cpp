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
}

void Anim::load_init(
    ivec size, usize image_count, usize layer_count, usize frame_count
) noexcept {
  this->size = size;
  this->type = ColorType::RGBA8;

  this->images.load_init(
      // NOLINTNEXTLINE
      image_count, (usize)size.x * (usize)size.y * sizeof(usize)
  );
  this->timeline.load_init(layer_count, frame_count);
}

void Anim::load_layer(usize index, LayerInfo layer_info) noexcept {
  this->timeline.load_layer(index, layer_info);
}

void Anim::load_frame(usize index, usize id, usize* image_ids) noexcept {
  this->timeline.load_frame(index, id, image_ids);
}

void Anim::load_image(usize index, usize id, data_ptr pixels) noexcept {
  this->images.load_image(index, id, pixels);
}

void Anim::load_finish() noexcept {
  this->images.load_finish();
}

void Anim::copy(const Anim& other) noexcept {
  if (this == &other) {
    return;
  }

  this->images.copy(other.images);
  this->timeline.copy(other.timeline);

  this->size = other.size;
  this->type = other.type;
}

void Anim::minicopy(const Anim& other) noexcept {
  if (this == &other) {
    return;
  }

  this->images.minicopy(other.images);
  this->timeline.copy(other.timeline);

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

usize Anim::get_layer_count() const noexcept {
  return this->timeline.get_layer_count();
}

usize Anim::get_frame_count() const noexcept {
  return this->timeline.get_frame_count();
}

Image Anim::get_image(usize id) noexcept {
  return Image{this->images.get_pixels(id), this->size, this->type, id};
}

Image Anim::get_image(usize frame_id, usize layer_index) noexcept {
  auto id = this->get_image_id(frame_id, layer_index);
  return Image{this->images.get_pixels(id), this->size, this->type, id};
}

Image Anim::get_image_fast(usize id) const noexcept {
  return Image{this->images.get_pixels_fast(id), this->size, this->type, id};
}

Image Anim::get_image_fast(usize frame_id, usize layer_index) const noexcept {
  auto id = this->get_image_id(frame_id, layer_index);
  return Image{this->images.get_pixels_fast(id), this->size, this->type, id};
}

data_ptr Anim::get_pixels(usize id) noexcept {
  return this->images.get_pixels(id);
}

data_ptr Anim::get_pixels(usize frame_id, usize layer_index) noexcept {
  return this->images.get_pixels(this->get_image_id(frame_id, layer_index));
}

data_ptr Anim::get_pixels_fast(usize id) const noexcept {
  return this->images.get_pixels_fast(id);
}

data_ptr
Anim::get_pixels_fast(usize frame_id, usize layer_index) const noexcept {
  return this->images.get_pixels_fast(this->get_image_id(frame_id, layer_index)
  );
}

void Anim::get_pixels_slow(usize id, data_ptr pixels) const noexcept {
  this->images.get_pixels_slow(id, pixels);
}

void Anim::get_pixels_slow(usize frame_id, usize layer_index, data_ptr pixels)
    const noexcept {
  this->images.get_pixels_slow(
      this->get_image_id(frame_id, layer_index), pixels
  );
}

usize Anim::get_image_id(usize frame_id, usize layer_index) const noexcept {
  return this->timeline.get_image_id(frame_id, layer_index);
}

const LayerInfo& Anim::get_layer_info(usize index) const noexcept {
  return this->timeline.get_layer_info(index);
}

const c8* Anim::get_layer_name(usize index) const noexcept {
  return this->timeline.get_layer_name(index);
}

bool Anim::is_layer_visible(usize index) const noexcept {
  return this->timeline.is_layer_visible(index);
}

void Anim::get_flatten(
    usize frame_id, usize start_layer, usize end_layer,
    ds::vector<rgba8>& pixels
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

    // NOTE: Might abort, must precall a load of images to cache
    img_ptr = (rgba8*)this->images.get_pixels_fast(frame.get_image_id(i));
    // NOTE: Add blending calculations here
    for (i32 i = 0; i < pixels.get_size(); ++i) {
      if (img_ptr[i].a) {
        pixels[i] = img_ptr[i];
      }
    }
  }
}

const std::string& Anim::get_name() noexcept {
  return this->name;
}

Anim::operator bool() const noexcept {
  return this->images.get_ptr() != nullptr;
}

// === Modifiers === //

usize Anim::insert_layer(usize index) noexcept {
  assert(index >= 0 && index <= this->timeline.get_layer_count());

  // Create a new layer in the layers data
  auto id = this->images.create_image();

  // Insert it into the first frame
  this->timeline.insert_layer(index, id);

  return id;
}

bool Anim::toggle_layer_visibility(usize layer_index) noexcept {
  return this->timeline.toggle_layer_visibility(layer_index);
}

void Anim::write_pixels_to_disk(usize id) const noexcept {
  this->images.write_pixels_to_disk(id);
}

// === Debugging === //

#ifndef NDEBUG

void Anim::print_metadata() const noexcept {
  if (!logger::lock(logger::Level::DEBUG_LVL, "Animation Medata")) {
    return;
  }

  logger::print("  Size: (%d, %d)\n", size.x, size.y);
  logger::print("  Image Count: %u\n", this->images.get_disk_size());
  logger::print("  Layer Count: %u\n", this->timeline.get_layer_count());
  logger::print("  Frame Count: %u\n", this->timeline.get_frame_count());

  logger::unlock();
}

void Anim::print_timeline_info_metadata() const noexcept {
  this->timeline.print_metadata();
}

void Anim::print_images_memory() const noexcept {
  if (!logger::lock(logger::Level::DEBUG_LVL, "Animation Images Memory")) {
    return;
  }

  usize mod = this->size.x * get_color_type_size(this->type);
  usize psize = mod * this->size.y;
  for (auto it = this->images.get_iter(); it; ++it) {
    logger::print("  Image Id: %u ; Ptr: %p", it.get_id(), it.get_data());
    for (usize i = 0U; i < psize; ++i) {
      if (i % mod == 0U) {
        logger::print("\n");
      }
      logger::print("%02x", it.get_data()[i]);
    }
    logger::print("\n");
  }

  logger::unlock();
}

void Anim::print_images_disk() const noexcept {
  if (!logger::lock(logger::Level::DEBUG_LVL, "Animation Images Disk")) {
    return;
  }

  ds::vector<u8> pixels{};
  pixels.resize(this->size.x * this->size.y * get_color_type_size(this->type));

  usize mod = this->size.x * get_color_type_size(this->type);
  for (usize id = 1U; id < this->images.get_disk_capacity(); ++id) {
    this->images.get_pixels_slow(id, pixels.get_data());

    logger::print("  Image Id: %u", id);
    for (usize i = 0U; i < pixels.get_size(); ++i) {
      if (i % mod == 0U) {
        logger::print("\n");
      }
      logger::print("%02x", pixels[i]);
    }
    logger::print("\n");
  }

  logger::unlock();
}

void Anim::print_timeline_info() const noexcept {
  if (!logger::lock(logger::Level::DEBUG_LVL, "Animation Timeline Info")) {
    return;
  }

  logger::print("=== Layers ===\n");
  for (usize i = 0U; i < this->timeline.get_layer_count(); ++i) {
    auto layer_info = this->timeline.get_layer_info(i);
    logger::print(
        "  Name: %s ; Opacity: %02x\n", layer_info.name, layer_info.opacity
    );
  }

  logger::print("=== Frames ===\n");
  for (auto it = this->timeline.get_frame_iter(); it; ++it) {
    logger::print("  Id: %u ; Images:\n ", it.get_id());
    for (usize i = 0U; i < this->timeline.get_layer_count(); ++i) {
      logger::print(" %3u", it.get_image_id(i));
    }
    logger::print("\n");
  }

  logger::unlock();
}

#endif

} // namespace draw

