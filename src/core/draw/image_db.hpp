/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-29
 *==========================*/

#ifndef PXL_DRAW_IMAGE_DB_HPP
#define PXL_DRAW_IMAGE_DB_HPP

#include "./types.hpp"
#include "types.hpp"

namespace draw {

// Forward iterator for ImageDb
class ImageDbIter {
public:
  ImageDbIter() noexcept = default;
  ImageDbIter(data_ptr ptr, i32 size, i32 capacity, i32 bytes) noexcept;

  ImageDbIter& operator++();

  [[nodiscard]] u32 get_id() const noexcept;
  [[nodiscard]] data_ptr get_data() const noexcept;

  explicit operator bool() const noexcept;

private:
  u32* id_ptr = nullptr;
  data_ptr image_ptr = nullptr;
  i32 size = 0;
  i32 bytes = 0;
};

// SOA Images Db
class ImageDb {
public:
  ImageDb() noexcept = default;
  ImageDb(const ImageDb&) noexcept = delete;
  ImageDb& operator=(const ImageDb&) noexcept = delete;

  void init(i32 bytes, i32 capacity) noexcept;
  ImageDb(ImageDb&& rhs) noexcept;
  ImageDb& operator=(ImageDb&& rhs) noexcept;
  void copy(const ImageDb& other) noexcept;
  void minicopy(const ImageDb& other) noexcept;
  ~ImageDb() noexcept;

  [[nodiscard]] data_ptr get_ptr() const noexcept;
  [[nodiscard]] data_ptr get_image(u32 id) const noexcept;
  [[nodiscard]] i32 get_size() const noexcept;
  [[nodiscard]] i32 get_capacity() const noexcept;
  [[nodiscard]] i32 get_bytes_size() const noexcept;

  [[nodiscard]] ImageDbIter get_iter() const noexcept;

  void create_layer(u32 new_id) noexcept;

private:
  // TODO: Change datatypes (64 bit) to support higher sizes
  data_ptr ptr = nullptr;
  i32 insert = 0;
  i32 last = 0;
  i32 capacity = 0;
  i32 size = 0;

  i32 bytes = 0;
  i32 alloc_size = 0;

  // === Copy Helpers === //
  void copy_empty(const ImageDb& other) noexcept;
  void copy_normal(const ImageDb& other) noexcept;
  void copy_overfit(const ImageDb& other) noexcept;
  void copy_grow(const ImageDb& other) noexcept;

  // === Memory === //
  void allocate(i32 timeline_alloc_size) noexcept;
  void reallocate(i32 new_timeline_alloc_size) noexcept;
};

} // namespace draw

#endif

