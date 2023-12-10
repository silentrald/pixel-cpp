/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-19
 *===============================*/

#include "./arena.hpp"
#include <cassert>
#include <cstdlib>

namespace history {

Arena::Arena() noexcept : data(nullptr), cursor(0U), alloc_size(0U) {}

error_code Arena::init(usize alloc_size) noexcept {
  this->alloc_size = alloc_size;
  this->cursor = 0U;

  this->data = static_cast<u8*>(std::malloc(alloc_size)); // NOLINT
  return this->data == nullptr ? error_code::BAD_ALLOC : error_code::OK;
}

Arena::Arena(Arena&& rhs) noexcept
    : data(rhs.data), cursor(rhs.cursor), alloc_size(rhs.alloc_size) {
  rhs.data = nullptr;
}

Arena& Arena::operator=(Arena&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->data = rhs.data;
  rhs.data = nullptr;

  this->cursor = rhs.cursor;
  this->alloc_size = rhs.alloc_size;

  return *this;
}

Arena::~Arena() noexcept {
  if (this->data) {
    std::free(this->data); // NOLINT
    this->data = nullptr;
  }
}

void Arena::set_cursor_with_ptr(void* new_data) noexcept {
  assert(this->data != nullptr);
  assert(new_data != nullptr);
  assert(new_data >= this->data && new_data < this->data + this->alloc_size);

  this->cursor = static_cast<u8*>(new_data) - this->data;
}

void* Arena::allocate(usize size) noexcept {
  assert(this->data != nullptr);

  u8* ptr = this->data;
  if (size > this->get_available()) {
    this->cursor = size;
  } else {
    ptr += this->cursor;
    this->cursor += size;
  }

  return ptr;
}

void* Arena::get_data() const noexcept {
  assert(this->data != nullptr);
  return this->data;
}

void* Arena::get_cursor_data() const noexcept {
  assert(this->data != nullptr);
  return this->data + this->cursor;
}

usize Arena::get_available() const noexcept {
  return this->alloc_size - this->cursor;
}

void Arena::reset() noexcept {
  this->cursor = 0U;
}

} // namespace history

