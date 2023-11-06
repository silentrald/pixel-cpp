/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-10
 *==========================*/

#include "./disk_backup.hpp"

namespace draw {

DiskBackup::DiskBackup(DiskBackup&& rhs) noexcept : fp(rhs.fp) {
  rhs.fp = nullptr;
}

DiskBackup& DiskBackup::operator=(DiskBackup&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  if (this->fp) {
    fclose(this->fp);
  }

  this->fp = rhs.fp;
  rhs.fp = nullptr;

  return *this;
}

DiskBackup::~DiskBackup() noexcept {
  if (this->fp) {
    fclose(this->fp);
    this->fp = nullptr;
  }
}

void DiskBackup::open_file(const c8* path, const c8* mode) noexcept {
  if (this->fp) {
    fclose(this->fp);
  }

  this->fp = fopen(path, mode);
  if (this->fp == nullptr) {
    std::abort();
  }
}

void DiskBackup::seek(u32 off) const noexcept {
  fseek(this->fp, off, SEEK_SET);
}

void DiskBackup::move(i32 off) const noexcept {
  fseek(this->fp, off, SEEK_CUR);
}

void DiskBackup::read(void* data, u32 size) const noexcept {
  fread(data, 1, size, this->fp);
}

[[nodiscard]] u32 DiskBackup::read_u32() const noexcept {
  u32 out = 0U;
  fread(&out, 4U, 1U, this->fp);
  return out;
}

void DiskBackup::write(void* data, u32 size) const noexcept {
  fwrite(data, 1, size, this->fp);
}

void DiskBackup::write_u32(u32 val) const noexcept {
  fwrite(&val, 4U, 1U, this->fp);
}

void DiskBackup::flush() const noexcept {
  fflush(this->fp);
}

} // namespace draw

