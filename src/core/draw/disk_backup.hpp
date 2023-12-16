/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-10
 *==========================*/

#ifndef PXL_DRAW_DISK_BACKUP_HPP
#define PXL_DRAW_DISK_BACKUP_HPP

#include "types.hpp"

namespace draw {

class DiskBackup {
public:
  DiskBackup() noexcept = default;
  DiskBackup(const DiskBackup&) noexcept = delete;
  DiskBackup& operator=(const DiskBackup&) noexcept = delete;

  DiskBackup(DiskBackup&& rhs) noexcept;
  DiskBackup& operator=(DiskBackup&& rhs) noexcept;
  ~DiskBackup() noexcept;

  [[nodiscard]] error_code open_file(const c8* path, const c8* mode) noexcept;

  [[nodiscard]] error_code seek(u32 off) const noexcept;
  [[nodiscard]] error_code move(i32 off) const noexcept;
  [[nodiscard]] i32 get_usize_bytes() const noexcept;

  [[nodiscard]] error_code read(void* data, u32 size) const noexcept;
  [[nodiscard]] expected<u32> read_u32() const noexcept;
  [[nodiscard]] expected<u64> read_u64() const noexcept;
  [[nodiscard]] expected<usize> read_usize() const noexcept;

  [[nodiscard]] error_code write(void* data, u32 size) const noexcept;
  [[nodiscard]] error_code write_u32(u32 uint) const noexcept;
  [[nodiscard]] error_code write_u64(u64 uint) const noexcept;
  [[nodiscard]] error_code write_usize(usize uint) const noexcept;
  [[nodiscard]] error_code flush() const noexcept;

private:
  FILE* fp = nullptr;
};

} // namespace draw

#endif
