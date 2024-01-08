/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-10
 *==========================*/

#include "./disk_backup.hpp"
#include <cassert>
#include <cstdio>

namespace draw {

DiskBackup::DiskBackup(DiskBackup&& rhs) noexcept : fp(rhs.fp) {
  rhs.fp = nullptr;
}

DiskBackup& DiskBackup::operator=(DiskBackup&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  if (this->fp) {
    fclose(this->fp); // NOLINT
  }

  this->fp = rhs.fp;
  rhs.fp = nullptr;

  return *this;
}

DiskBackup::~DiskBackup() noexcept {
  if (this->fp) {
    fclose(this->fp); // NOLINT
    this->fp = nullptr;
  }
}

error_code DiskBackup::open_file(const c8* path, const c8* mode) noexcept {
  if (this->fp) {
    fclose(this->fp); // NOLINT
  }

  this->fp = fopen(path, mode); // NOLINT
  return this->fp == nullptr ? error_code::FILE_NOT_FOUND : error_code::OK;
}

error_code DiskBackup::seek(u32 off) const noexcept {
  assert(this->fp != nullptr);
  return fseek(this->fp, off, SEEK_SET) == 0 ? error_code::OK
                                             : error_code::FILE_SEEK;
}

error_code DiskBackup::move(i32 off) const noexcept {
  assert(this->fp != nullptr);
  return fseek(this->fp, off, SEEK_CUR) == 0 ? error_code::OK
                                             : error_code::FILE_SEEK;
}

error_code DiskBackup::read(void* data, u32 size) const noexcept {
  assert(this->fp != nullptr);
  return fread(data, 1, size, this->fp) == size ? error_code::OK
                                                : error_code::FILE_READ;
}

expected<u32> DiskBackup::read_u32() const noexcept {
  assert(this->fp != nullptr);
  u32 out = 0U;

  if (fread(&out, 4U, 1U, this->fp) != 1) {
    return unexpected{error_code::FILE_READ};
  }

  return out;
}

expected<u64> DiskBackup::read_u64() const noexcept {
  assert(this->fp != nullptr);
  u64 out = 0U;

  if (fread(&out, sizeof(u64), 1U, this->fp) != 1) {
    return unexpected{error_code::FILE_READ};
  }

  return out;
}

expected<usize> DiskBackup::read_usize() const noexcept {
#ifdef BIT_32
  return this->read_u32();
#else
  return this->read_u64();
#endif
}

error_code DiskBackup::write(void* data, u32 size) const noexcept {
  assert(this->fp != nullptr);
  return fwrite(data, 1U, size, this->fp) == size ? error_code::OK
                                                  : error_code::FILE_WRITE;
}

error_code DiskBackup::write_u32(u32 uint) const noexcept {
  assert(this->fp != nullptr);
  return fwrite(&uint, 4U, 1U, this->fp) == 1 ? error_code::OK
                                              : error_code::FILE_WRITE;
}

error_code DiskBackup::write_u64(u64 uint) const noexcept {
  assert(this->fp != nullptr);
  return fwrite(&uint, sizeof(u64), 1U, this->fp) == 1 ? error_code::OK
                                                       : error_code::FILE_WRITE;
}

error_code DiskBackup::write_usize(usize uint) const noexcept {
#ifdef BIT_32
  return this->write_u32(uint);
#else
  return this->write_u64(uint);
#endif
}

error_code DiskBackup::flush() const noexcept {
  assert(this->fp != nullptr);
  return fflush(this->fp) == 0 ? error_code::OK : error_code::FILE_WRITE;
}

} // namespace draw
