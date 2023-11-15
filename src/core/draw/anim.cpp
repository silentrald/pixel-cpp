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

error_code Anim::init(ivec size, ColorType type) noexcept {
  // NOTE: Size cap can be bigger if datatypes are changed to 64 bit datatypes
  //   but MAX_LENGTH is sufficient enough
  if (size.x < 0 || size.y < 0 || size.x > MAX_LENGTH || size.y > MAX_LENGTH) {
    logger::fatal("Size (%d, %d) is either to big or negative", size.x, size.y);
    std::abort();
  }

  this->size = size;
  this->type = type;

  // TODO: Compute whether layers_max_bytes or layers_capacity is set
  TRY(this->images.init(size.x * size.y * get_color_type_size(this->type), 64U)
  );
  TRY(this->timeline.init());
  return error_code::OK;
}

error_code Anim::load_init(
    ivec size, usize image_count, usize layer_count, usize frame_count
) noexcept {
  this->size = size;
  this->type = ColorType::RGBA8;

  TRY(this->images.load_init(
      // NOLINTNEXTLINE
      image_count, (usize)size.x * (usize)size.y * sizeof(usize)
  ));
  TRY(this->timeline.load_init(layer_count, frame_count));
  return error_code::OK;
}

void Anim::load_layer(usize index, LayerInfo layer_info) noexcept {
  this->timeline.load_layer(index, layer_info);
}

void Anim::load_frame(usize index, usize id, usize* image_ids) noexcept {
  this->timeline.load_frame(index, id, image_ids);
}

error_code Anim::load_image(usize index, usize id, data_ptr pixels) noexcept {
  return this->images.load_image(index, id, pixels);
}

error_code Anim::load_finish() noexcept {
  return this->images.load_finish();
}

error_code Anim::copy(const Anim& other) noexcept {
  if (this == &other) {
    return error_code::OK;
  }

  TRY(this->images.copy(other.images));
  TRY(this->timeline.copy(other.timeline));

  this->size = other.size;
  this->type = other.type;
  return error_code::OK;
}

error_code Anim::minicopy(const Anim& other) noexcept {
  if (this == &other) {
    return error_code::OK;
  }

  TRY(this->images.minicopy(other.images));
  TRY(this->timeline.copy(other.timeline));

  this->size = other.size;
  this->type = other.type;
  return error_code::OK;
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

expected<Image> Anim::get_image(usize id) noexcept {
  auto* pixels = *TRY_RET(this->images.get_pixels(id), {}, to_unexpected);
  return Image{pixels, this->size, this->type, id};
};

expected<Image> Anim::get_image(usize frame_id, usize layer_index) noexcept {
  auto id = this->get_image_id(frame_id, layer_index);
  auto* pixels = *TRY_RET(this->images.get_pixels(id), {}, to_unexpected);
  return Image{pixels, this->size, this->type, id};
}

Image Anim::get_image_fast(usize id) const noexcept {
  return Image{this->images.get_pixels_fast(id), this->size, this->type, id};
}

Image Anim::get_image_fast(usize frame_id, usize layer_index) const noexcept {
  auto id = this->get_image_id(frame_id, layer_index);
  return Image{this->images.get_pixels_fast(id), this->size, this->type, id};
}

usize Anim::get_image_bytes_size() const noexcept {
  return this->images.get_bytes_size();
}

expected<data_ptr> Anim::get_pixels(usize id) noexcept {
  return this->images.get_pixels(id);
}

expected<data_ptr>
Anim::get_pixels(usize frame_id, usize layer_index) noexcept {
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

error_code Anim::get_pixels_slow(usize id, data_ptr pixels) const noexcept {
  return this->images.get_pixels_slow(id, pixels);
}

error_code Anim::get_pixels_slow(
    usize frame_id, usize layer_index, data_ptr pixels
) const noexcept {
  return this->images.get_pixels_slow(
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
    ds::vector<data_type>& pixels
) const noexcept {
  assert(frame_id != 0U);
  assert(
      pixels.get_size() ==
      this->size.x * this->size.y * get_color_type_size(this->type)
  );
  assert(start_layer <= end_layer);
  assert(start_layer >= 0 && start_layer < this->timeline.get_layer_count());
  assert(end_layer >= 0 && end_layer < this->timeline.get_layer_count());

  // NOTE: Only supports rgba8 for now
  usize psize = this->size.x * this->size.y;
  auto* pixels_ptr = (rgba8*)pixels.get_data();
  rgba8* img_ptr = nullptr;
  auto frame = this->timeline.get_frame(frame_id);
  for (usize i = start_layer; i <= end_layer; ++i) {
    if (!this->timeline.is_layer_visible(i)) {
      continue;
    }

    // NOTE: Might abort, must precall a load of images to cache
    img_ptr = (rgba8*)this->images.get_pixels_fast(frame.get_image_id(i));
    // NOTE: Add blending calculations here
    for (i32 i = 0; i < psize; ++i) {
      if (img_ptr[i].a) {
        pixels_ptr[i] = img_ptr[i];
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

expected<usize> Anim::insert_layer(usize index) noexcept {
  assert(index >= 0 && index <= this->timeline.get_layer_count());

  // Create a new layer in the layers data
  auto id = *TRY_RET(this->images.create_image());

  // Insert it into the first frame
  TRY(this->timeline.insert_layer(index, id), {}, to_unexpected);

  return id;
}

bool Anim::toggle_layer_visibility(usize layer_index) noexcept {
  return this->timeline.toggle_layer_visibility(layer_index);
}

error_code Anim::write_pixels_to_disk(usize id) const noexcept {
  return this->images.write_pixels_to_disk(id);
}

// === Debugging === //

#ifndef NDEBUG

void Anim::print_metadata() const noexcept {
  logger::debug(
      "Animation Metadata\n"
      "  Size: (%d, %d)\n"
      "  Image Count: %u\n"
      "  Layer Count: %u\n"
      "  Frame Count: %u\n",
      size.x, size.y, this->images.get_disk_size(),
      this->timeline.get_layer_count(), this->timeline.get_frame_count()
  );
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
  ds::vector<u8> pixels{};
  TRY(pixels.resize(
          this->size.x * this->size.y * get_color_type_size(this->type)
      ),
      logger::warn("Could not resize pixels vector, skip printing"), to_void);

  if (!logger::lock(logger::Level::DEBUG_LVL, "Animation Images Disk")) {
    return;
  }

  usize mod = this->size.x * get_color_type_size(this->type);
  for (usize id = 1U; id < this->images.get_disk_capacity(); ++id) {
    TRY(
        this->images.get_pixels_slow(id, pixels.get_data()),
        { logger::unlock(); }, to_void
    );

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

