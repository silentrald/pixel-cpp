/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-29
 *==========================*/

#ifndef PXL_DRAW_IMAGE_DB_HPP
#define PXL_DRAW_IMAGE_DB_HPP

#include "./disk_backup.hpp"
#include "./types.hpp"
#include "core/ds/vector.hpp"
#include "types.hpp"

namespace draw {

class ImageDbIter;

// SOA Images Db
class ImageDb {
public:
  ImageDb() noexcept;
  ImageDb(const ImageDb&) noexcept = delete;
  ImageDb& operator=(const ImageDb&) noexcept = delete;

  void init(usize bytes, usize capacity) noexcept;

  void load_init(usize image_count, usize bytes) noexcept;
  void load_image(usize index, usize id, data_ptr pixels) noexcept;
  void load_finish() noexcept;

  ImageDb(ImageDb&& rhs) noexcept;
  ImageDb& operator=(ImageDb&& rhs) noexcept;
  void copy(const ImageDb& other) noexcept;
  void minicopy(const ImageDb& other) noexcept;
  ~ImageDb() noexcept;

  [[nodiscard]] data_ptr get_ptr() const noexcept;

  /**
   * Average retrieval of image data either cache hit on memory or cache miss on
   * disk.
   **/
  [[nodiscard]] data_ptr get_pixels(usize id) noexcept;

  /**
   * Fast retrieval of image data in memory.
   * If the image data is not found in memory, this will abort.
   * Call `get_image` first.
   *
   * @param id
   * @return data_ptr - pixel data
   **/
  [[nodiscard]] data_ptr get_pixels_fast(usize id) const noexcept;

  /**
   * Slow retrieval of image data in disk.
   *
   * @param id
   * @param pixels - output of the disk read
   **/
  void get_pixels_slow(usize id, data_ptr pixels) const noexcept;

  [[nodiscard]] usize get_size() const noexcept;
  [[nodiscard]] usize get_capacity() const noexcept;
  [[nodiscard]] usize get_bytes_size() const noexcept;
  [[nodiscard]] usize get_disk_size() const noexcept;
  [[nodiscard]] usize get_disk_capacity() const noexcept;

  [[nodiscard]] ImageDbIter get_iter() const noexcept;

  usize create_image() noexcept;
  void write_pixels_to_disk(usize id) const noexcept;

private:
  data_ptr ptr = nullptr;
  usize insert_index;
  usize head_index;
  usize tail_index;
  usize size = 0U;
  usize capacity = 0U;

  usize bytes = 0U;
  usize alloc_size = 0U;

  usize next_id = 1U;

  DiskBackup disk{};
  usize disk_size = 0U;
  usize disk_capacity = 0U;

  // === Copy Helpers === //
  void copy_empty(const ImageDb& other) noexcept;
  void copy_normal(const ImageDb& other) noexcept;
  void copy_overfit(const ImageDb& other) noexcept;
  void copy_grow(const ImageDb& other) noexcept;

  // === Memory === //
  void allocate(usize alloc_size) noexcept;

  // === Private Accessors === //

  struct Indexing {
    usize next = 0U;
    usize prev = 0U;
  };

  [[nodiscard]] inline usize* get_id_ptr() const noexcept;
  [[nodiscard]] inline Indexing* get_index_ptr() const noexcept;
  [[nodiscard]] inline data_ptr get_pixels_ptr() const noexcept;

  // === Disk Helper === //

  inline void seek_disk_id(usize id) const noexcept;
  inline void seek_disk_pixels(usize id) const noexcept;
};

// Forward iterator for ImageDb
class ImageDbIter {
public:
  ImageDbIter() noexcept = default;
  ImageDbIter(data_ptr ptr, usize size, usize capacity, usize bytes) noexcept;

  ImageDbIter& operator++();

  [[nodiscard]] usize get_id() const noexcept;
  [[nodiscard]] data_ptr get_data() const noexcept;

  explicit operator bool() const noexcept;

private:
  usize* id_ptr = nullptr;
  data_ptr image_ptr = nullptr;
  usize size = 0U;
  usize bytes = 0U;
};

} // namespace draw

#endif

