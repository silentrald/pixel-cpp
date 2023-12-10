/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-30
 *===============================*/

#include "./png.hpp"
#include "core/ds/vector.hpp"
#include "core/logger/logger.hpp"
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <png.h>
#include <pngconf.h>

namespace file {

class PngData {
public:
  PngData() noexcept = default;
  PngData(const PngData&) noexcept = delete;
  PngData& operator=(const PngData&) noexcept = delete;
  PngData(PngData&&) noexcept = delete;
  PngData& operator=(PngData&&) noexcept = delete;

  ~PngData() noexcept;

  bool init(const c8* path) noexcept;
  bool save_frame(const draw::Anim& anim, u32 frame_index) noexcept;

private:
  FILE* fp = nullptr;
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
};

PngData::~PngData() noexcept {
  if (this->fp) {
    fclose(this->fp); // NOLINT
    this->fp = nullptr;
  }

  if (this->png_ptr) {
    png_destroy_write_struct(
        &this->png_ptr, this->info_ptr ? &this->info_ptr : nullptr
    );
    this->png_ptr = nullptr;
    this->info_ptr = nullptr;
  }
}

// === Setters === //
bool PngData::init(const c8* path) noexcept {
  this->fp = fopen(path, "wb"); // NOLINT
  if (this->fp == nullptr) {
    return false;
  }

  this->png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (this->png_ptr == nullptr) {
    return false;
  }

  this->info_ptr = png_create_info_struct(this->png_ptr);
  if (this->info_ptr == nullptr) {
    return false;
  }

  // NOLINTNEXTLINE
  if (setjmp(png_jmpbuf(this->png_ptr))) {
    return false;
  }

  png_init_io(this->png_ptr, this->fp);
  return true;
}

bool PngData::save_frame(const draw::Anim& anim, u32 frame_index) noexcept {
  png_set_IHDR(
      this->png_ptr, this->info_ptr, anim.get_width(), anim.get_height(), 8,
      PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(this->png_ptr, this->info_ptr);

  // NOLINTNEXTLINE
  auto* row_pointers =
      // NOLINTNEXTLINE
      static_cast<png_bytep*>(std::malloc(sizeof(png_bytep) * anim.get_height())
      );
  if (row_pointers == nullptr) {
    logger::error("No more memory");
    return false;
  }

  i32 row_size = png_get_rowbytes(this->png_ptr, this->info_ptr);
  png_bytep row_pointer = nullptr;
  for (i32 y = 0; y < anim.get_height(); ++y) {
    // NOLINTNEXTLINE
    row_pointer = (png_bytep)std::malloc(row_size);
    if (row_pointer != nullptr) {
      row_pointers[y] = row_pointer;
      continue;
    }

    // Fail case for bad alloc
    for (i32 y2 = y - 1; y2 >= 0; --y2) {
      free(row_pointers[y2]); // NOLINT
    }
    logger::error("No more memory");
    return false;
  }

  ds::vector<draw::data_type> pixels{};
  TRY_ABORT(
      pixels.resize(anim.get_image_db().get_bytes_size()),
      "Could not resize temp storage"
  );
  anim.get_flatten(frame_index, 0, anim.get_layer_count() - 1, pixels);
  for (i32 y = 0; y < anim.get_height(); ++y) {
    row_pointer = row_pointers[y];
    // NOLINTNEXTLINE
    std::memcpy(row_pointer, pixels.get_data() + y * row_size, row_size);
  }
  png_write_image(this->png_ptr, row_pointers);
  png_write_end(this->png_ptr, nullptr);

  for (i32 y = 0; y < anim.get_height(); ++y) {
    free(row_pointers[y]); // NOLINT
  }
  free(row_pointers); // NOLINT

  return true;
}

// === Main Function === //

void Png::export_frame(const draw::Anim& anim, u32 frame_index, const c8* path)
    const noexcept {
  PngData data{};
  if (!data.init(path)) {
    logger::error("File could not be created");
    return;
  }

  if (!data.save_frame(anim, frame_index)) {
    logger::error("PNG could not be saved");
    return;
  }
}

} // namespace file
