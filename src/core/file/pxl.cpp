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

// BUG: May crash if the file is getting deleted, handle the fprintf error when
// expected is impl
// NOTE: Caching is not supported here yet, assuming the whole
// data is in memory
void Pxl::save(const draw::Anim& anim, const c8* path) const noexcept {
  FILE* fp = fopen(path, "w"); // NOLINT
  if (fp == nullptr) {
    logger::error("Could not write to %s", path);
    return;
  }

  const draw::ImageDb& images = anim.get_image_db();

  // Metadata
  // NOLINTNEXTLINE
  fprintf(
      fp, "MDT\n%d %d %u %u %u\n", anim.get_width(), anim.get_height(),
      images.get_size(), anim.get_layer_count(), anim.get_frame_count()
  );

  // Layers Section
  fprintf(fp, "LYR\n"); // NOLINT
  for (draw::usize i = 0U; i < anim.get_layer_count(); ++i) {
    const auto& layer_info = anim.get_layer_info(i);
    // NOLINTNEXTLINE
    fprintf(fp, "%02x %s\n", layer_info.opacity, layer_info.name);
  }

  // Frames Section
  {
    fprintf(fp, "FRM\n"); // NOLINT
    const draw::TimelineInfo& timeline_info = anim.get_timeline_info();
    for (auto it = timeline_info.get_frame_iter(); it; ++it) {
      fprintf(fp, "%u", it.get_id()); // NOLINT
      for (i32 i = 0; i < timeline_info.get_layer_count(); ++i) {
        fprintf(fp, " %u", it.get_image_id(i)); // NOLINT
      }
      fprintf(fp, "\n"); // NOLINT
    }
  }

  // Images Section
  auto* pixels = (draw::data_ptr)malloc(images.get_bytes_size()); // NOLINT
  if (pixels == nullptr) {
    logger::fatal("Could not create temporary pixels");
    std::abort();
  }

  fprintf(fp, "IMG\n"); // NOLINT
  for (draw::usize id = 1U; id <= images.get_disk_capacity(); ++id) {
    images.get_pixels_slow(id, pixels);

    fprintf(fp, "%u ", id);                         // NOLINT
    fwrite(pixels, 1, images.get_bytes_size(), fp); // NOLINT
    fprintf(fp, "\n");                              // NOLINT
  }
  free(pixels); // NOLINT

  fclose(fp); // NOLINT
}

draw::Anim Pxl::load(const c8* path) const noexcept {
  FILE* fp = fopen(path, "r"); // NOLINT
  if (fp == nullptr) {
    logger::error("Could not read %s", path);
    return {};
  }

  draw::Anim anim{};

  // Metadata
  draw::usize image_count = 0U;
  {
    ivec size{};
    draw::usize layer_count = 0U;
    draw::usize frame_count = 0U;
    // NOLINTNEXTLINE
    fscanf(
        fp, "%*s %d %d %u %u %u", &size.x, &size.y, &image_count, &layer_count,
        &frame_count
    );

    anim.load_init(size, image_count, layer_count, frame_count);
  }

  // Layer Section
  {
    fscanf(fp, "%*s"); // NOLINT
    draw::LayerInfo layer_info{};

    for (draw::usize i = 0U; i < anim.get_layer_count(); ++i) {
      // NOLINTNEXTLINE
      fscanf(fp, "%02x %51[^\n]", &layer_info.opacity, layer_info.name);
      anim.load_layer(i, layer_info);
    }
  }

  // Frame Section
  {
    fscanf(fp, "%*s"); // NOLINT

    draw::usize id = 0U;
    ds::vector<draw::usize> image_ids{};
    image_ids.resize(anim.get_width() * anim.get_height());

    for (draw::usize i = 0U; i < anim.get_frame_count(); ++i) {
      fscanf(fp, "%u ", &id); // NOLINT

      for (draw::usize j = 0U; j < anim.get_layer_count(); ++j) {
        // NOLINTNEXTLINE
        fscanf(fp, "%u ", image_ids.get_data() + j);
      }

      anim.load_frame(i, id, image_ids.get_data());
    }
  }

  // Image Section
  {
    fscanf(fp, "%*s"); // NOLINT

    ds::vector<u8> pixels{};
    // NOLINTNEXTLINE
    pixels.resize(anim.get_width() * anim.get_height() * sizeof(rgba8));
    u32 id = 0U;

    for (i32 i = 0; i < image_count; ++i) {
      fscanf(fp, "%u ", &id);                              // NOLINT
      fread(pixels.get_data(), 1U, pixels.get_size(), fp); // NOLINT

      anim.load_image(i, id, pixels.get_data());
    }
  }

  fclose(fp); // NOLINT

  return anim;
}

} // namespace file

