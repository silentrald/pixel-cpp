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
  for (i32 i = 0; i < anim.get_layer_count(); ++i) {
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

  // Metadata
  ivec size{};
  i32 image_count = 0;
  i32 layer_count = 0;
  i32 frame_count = 0;
  // NOLINTNEXTLINE
  fscanf(
      fp, "%*s %d %d %d %d %d", &size.x, &size.y, &image_count, &layer_count,
      &frame_count
  );

  logger::debug(
      "metadata: %d %d %d %d %d", size.x, size.y, image_count, layer_count,
      frame_count
  );

  // Layer Section
  fscanf(fp, "%*s"); // NOLINT
  draw::LayerInfo layer_info{};

  for (i32 i = 0; i < layer_count; ++i) {
    fscanf(fp, "%02x %51[^\n]", &layer_info.opacity, layer_info.name); // NOLINT

    logger::debug("Layer: %s %02x", layer_info.name, layer_info.opacity);
  }

  // Frame Section
  {
    fscanf(fp, "%*s"); // NOLINT
    u32 id = 0U;

    fscanf(fp, "%u ", &id); // NOLINT
    logger::debug("%u", id);

    for (i32 i = 0; i < layer_count; ++i) {
      fscanf(fp, "%u ", &id); // NOLINT
      printf("%u ", id);
    }
    printf("\n");
  }

  // Image Section
  {
    fscanf(fp, "%*s"); // NOLINT
    ds::vector<u8> data{};
    data.resize(size.x * size.y * sizeof(rgba8));
    u32 id = 0U;
    for (i32 i = 0; i < image_count; ++i) {
      fscanf(fp, "%u ", &id);                         // NOLINT
      fread(data.get_data(), 1, data.get_size(), fp); // NOLINT

      // Store this in memory
      logger::debug("%u", id);
      for (i32 i = 0; i < data.get_size(); ++i) {
        printf("%02x ", data[i]);
      }
      printf("\n");
    }
  }

  fclose(fp); // NOLINT

  draw::Anim anim{};
  return anim;
}

} // namespace file

