/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-05
 *===============================*/

#include "./pxl.hpp"
#include "core/draw/timeline_info.hpp"
#include "core/ds/vector.hpp"
#include "core/logger/logger.hpp"
#include <cstdio>
#include <cstdlib>

namespace file {

void Pxl::set_auto_save(u32 auto_save) noexcept {
  this->auto_save = auto_save;
}

bool Pxl::will_auto_save() noexcept {
  if (this->auto_save == 0U) {
    return false;
  }

  ++this->actions;
  return this->actions == this->auto_save;
}

error_code Pxl::force_auto_save(const draw::Anim& anim) noexcept {
  this->actions = 0;
  return this->save(anim, "save.pxl");
}

error_code Pxl::try_auto_save(const draw::Anim& anim) noexcept {
  if (this->auto_save == 0U) {
    return error_code::OK;
  }

  ++this->actions;
  if (this->actions < this->auto_save) {
    return error_code::OK;
  }

  this->actions = 0;
  return this->save(anim, "save.pxl");
}

// NOLINTNEXTLINE
#define FILE_PRINT(...)                                                        \
  if (fprintf(__VA_ARGS__) < 0) {                                              \
    return error_code::FILE_WRITE;                                             \
  }

// NOLINTNEXTLINE
error_code Pxl::save(const draw::Anim& anim, const c8* path) noexcept {
  FILE* fp = fopen(path, "w"); // NOLINT
  if (fp == nullptr) {
    logger::error("Could not write to %s", path);
    return error_code::FILE_NOT_FOUND;
  }

  const draw::ImageDb& images = anim.get_image_db();

  // === Metadata === //
  FILE_PRINT(
      fp, "MDT\n%d %d " USIZE_FMT " " USIZE_FMT " " USIZE_FMT "\n",
      anim.get_width(), anim.get_height(), images.get_disk_size(),
      anim.get_layer_count(), anim.get_frame_count()
  );

  // === Layers Section === //
  FILE_PRINT(fp, "LYR\n");

  for (usize i = 0U; i < anim.get_layer_count(); ++i) {
    const auto& layer_info = anim.get_layer_info(i);
    FILE_PRINT(fp, "%02x %s\n", layer_info.opacity, layer_info.name);
  }

  // === Frames Section === //
  FILE_PRINT(fp, "FRM\n");
  {
    const draw::TimelineInfo& timeline_info = anim.get_timeline_info();
    for (auto it = timeline_info.get_frame_iter(); it; ++it) {
      FILE_PRINT(fp, USIZE_FMT, it.get_id());
      for (i32 i = 0; i < timeline_info.get_layer_count(); ++i) {
        FILE_PRINT(fp, " " USIZE_FMT, it.get_image_id(i));
      }
      FILE_PRINT(fp, "\n");
    }
  }

  // === Images Section === //
  if (this->pixels.get_capacity() < images.get_bytes_size()) {
    TRY(this->pixels.resize(images.get_bytes_size()));
  }

  FILE_PRINT(fp, "IMG\n");
  for (usize id = 1U; id <= images.get_disk_capacity(); ++id) {
    TRY(images.get_pixels_slow(id, this->pixels.get_data()));

    FILE_PRINT(fp, USIZE_FMT " ", id);
    if (fwrite(this->pixels.get_data(), 1, images.get_bytes_size(), fp) !=
        images.get_bytes_size()) {
      return error_code::FILE_WRITE;
    }
    FILE_PRINT(fp, "\n");
  }

  if (fclose(fp) != 0) { // NOLINT
    return error_code::FILE_WRITE;
  }
  return error_code::OK;
}

#undef FILE_PRINT

// NOLINTNEXTLINE
#define FILE_SCAN(...)                                                         \
  /* NOLINTNEXTLINE */                                                         \
  if (fscanf(__VA_ARGS__) < 0) {                                               \
    return unexpected{error_code::FILE_READ};                                  \
  }

// NOLINTNEXTLINE
expected<draw::Anim> Pxl::load(const c8* path) noexcept {
  FILE* fp = fopen(path, "r"); // NOLINT
  if (fp == nullptr) {
    logger::error("Could not read %s", path);
    return unexpected{error_code::FILE_NOT_FOUND};
  }

  draw::Anim anim{};

  // Metadata
  usize image_count = 0U;
  {
    ivec size{};
    usize layer_count = 0U;
    usize frame_count = 0U;

    FILE_SCAN(
        fp, "%*s %d %d" USIZE_FMT USIZE_FMT USIZE_FMT, &size.x, &size.y,
        &image_count, &layer_count, &frame_count
    );

    TRY(anim.load_init(size, image_count, layer_count, frame_count), {},
        to_unexpected);
  }

  // Layer Section
  {
    FILE_SCAN(fp, "%*s");
    draw::LayerInfo layer_info{};
    for (usize i = 0U; i < anim.get_layer_count(); ++i) {
      FILE_SCAN(fp, "%02x %51[^\n]", &layer_info.opacity, layer_info.name);
      anim.load_layer(i, layer_info);
    }
  }

  // Frame Section
  {
    FILE_SCAN(fp, "%*s");

    usize id = 0U;
    ds::vector<usize> image_ids{};
    TRY(image_ids.resize(anim.get_width() * anim.get_height()), {},
        to_unexpected);

    for (usize i = 0U; i < anim.get_frame_count(); ++i) {
      FILE_SCAN(fp, USIZE_FMT " ", &id);
      for (usize j = 0U; j < anim.get_layer_count(); ++j) {
        FILE_SCAN(fp, USIZE_FMT " ", image_ids.get_data() + j);
      }
      anim.load_frame(i, id, image_ids.get_data());
    }
  }

  // Image Section
  {
    FILE_SCAN(fp, "%*s");

    if (this->pixels.get_capacity() < anim.get_image_db().get_bytes_size()) {
      TRY(this->pixels.resize(anim.get_image_db().get_bytes_size()), {},
          to_unexpected);
    }
    u32 id = 0U;

    for (i32 i = 0; i < image_count; ++i) {
      FILE_SCAN(fp, USIZE_FMT " ", &id);
      if (fread(pixels.get_data(), 1U, pixels.get_size(), fp) !=
          pixels.get_size()) {
        return unexpected{error_code::FILE_READ};
      }
      TRY(anim.load_image(i, id, pixels.get_data()), {}, to_unexpected);
    }
  }

  if (fclose(fp) != 0) { // NOLINT
    return unexpected{error_code::FILE_READ};
  }

  return anim;
}

#undef FILE_SCAN

} // namespace file

