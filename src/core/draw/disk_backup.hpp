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

  void open_file(const c8* path, const c8* mode) noexcept;

  void seek(u32 off) const noexcept;
  void move(i32 off) const noexcept;

  void read(void* data, u32 size) const noexcept;
  [[nodiscard]] u32 read_u32() const noexcept;

  void write(void* data, u32 size) const noexcept;
  void write_u32(u32 val) const noexcept;
  void flush() const noexcept;

private:
  FILE* fp = nullptr;
};

} // namespace draw

#endif
