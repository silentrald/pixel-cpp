/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-05
 *===============================*/

#include "./pxl.hpp"
#include "core/logger/logger.hpp"
#include <cstdio>

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

  // Metadata
  // NOLINTNEXTLINE
  fprintf(
      fp, "MDT\n%d %d %d %d\n", anim.get_width(), anim.get_height(),
      anim.get_layer_count(), anim.get_frame_count()
  );

  // Images Section
  {
    fprintf(fp, "IMG\n"); // NOLINT
    const draw::ImageDb& images = anim.get_image_db();
    for (auto it = images.get_iter(); it; ++it) {
      fprintf(fp, "%u ", it.get_id());                       // NOLINT
      fwrite(it.get_data(), 1, images.get_bytes_size(), fp); // NOLINT
      fprintf(fp, "\n");                                     // NOLINT
    }
  }

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

  fclose(fp); // NOLINT
}

} // namespace file

