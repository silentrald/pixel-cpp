/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-24
 *===============================*/

#ifndef PXL_DS_VECTOR_HPP
#define PXL_DS_VECTOR_HPP

#include "types.hpp"
#include <cassert>
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

namespace ds {

inline const usize VECTOR_INITIAL_SIZE = 4;
inline const usize VECTOR_K = 2;

/**
 * Self implemented vector class, std::vector crashes view data impls.
 * TODO: Add bool space efficient optimization
 **/
template <typename T> class vector {
private:
  using val = T;
  using ptr = T*;
  using ref = T&;
  using cref = const T&;
  using rref = T&&;

public:
  vector(const vector&) noexcept = delete;
  vector& operator=(const vector&) noexcept = delete;

  vector() noexcept : data(nullptr), top(0), capacity(0){};

  ~vector() noexcept {
    if (!this->data) {
      return;
    }

    if constexpr (std::is_fundamental<T>::value) {
      std::free(this->data); // NOLINT
    } else {
      delete[] this->data;
    }
    this->data = nullptr;
  }

  vector(vector&& rhs) noexcept
      : data(rhs.data), top(rhs.top), capacity(rhs.capacity) {
    rhs.data = nullptr;
  }

  vector& operator=(vector&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->data = rhs.data;
    this->top = rhs.top;
    this->capacity = rhs.capacity;

    rhs.data = nullptr;

    return *this;
  }

  // === Info === //

  [[nodiscard]] ptr get_data() const noexcept {
    return this->data;
  }

  /**
   * Number of elements within the vector
   **/
  [[nodiscard]] usize get_size() const noexcept {
    return this->top;
  }

  /**
   * Check if the vector does not have any elements
   **/
  [[nodiscard]] bool is_empty() const noexcept {
    return this->top == 0;
  }

  /**
   * The maximum number of elements in the vector, this can change if a vector
   *  mutation happens
   **/
  [[nodiscard]] usize get_capacity() const noexcept {
    return this->capacity;
  }

  // === Modifiers === //

  [[nodiscard]] error_code resize(usize new_size) noexcept {
    assert(new_size >= 0);

    auto code =
        this->data ? this->reallocate(new_size) : this->allocate(new_size);
    this->top = new_size;
    return code;
  }

  [[nodiscard]] error_code push_back(cref elem) noexcept {
    TRY(this->check_allocation());

    this->data[this->top] = elem;
    ++this->top;

    return error_code::OK;
  }

  [[nodiscard]] error_code push_back(rref elem) noexcept {
    TRY(this->check_allocation());

    this->data[this->top] = std::move(elem);
    ++this->top;

    return error_code::OK;
  }

  rref pop() noexcept {
    assert(this->top > 0);

    --this->top;
    return std::move(this->data[this->top + 1]);
  }

  [[nodiscard]] error_code insert(usize index, rref elem) noexcept {
    assert(index <= this->top);
    TRY(this->check_allocation());

    // Shift
    for (usize i = this->top; i > index; --i) {
      this->data[i] = std::move(this->data[i - 1]);
    }
    this->data[index] = std::move(elem);
    ++this->top;

    return error_code::OK;
  }

  void remove(usize index) noexcept {
    assert(this->top > 0);

    for (usize i = index + 1; i < this->top; ++i) {
      this->data[i - 1] = std::move(this->data[i]);
    }
    --this->top;
  }

  void clear() noexcept {
    // TODO: Check this if clearing the data is needed
    this->top = 0;
  }

  // === Accessing === //

  [[nodiscard]] ref operator[](usize index) noexcept {
    assert(index < this->top);
    return this->data[index];
  }

  [[nodiscard]] cref operator[](usize index) const noexcept {
    assert(index < this->top);
    return this->data[index];
  }

  [[nodiscard]] ref back() noexcept {
    assert(this->top > 0);
    return this->data[this->top - 1];
  }

  [[nodiscard]] cref back() const noexcept {
    assert(this->top > 0);
    return this->data[this->top - 1];
  }

  [[nodiscard]] ref front() noexcept {
    assert(this->top > 0);
    return *this->data;
  }

  [[nodiscard]] cref front() const noexcept {
    assert(this->top > 0);
    return *this->data;
  }

private:
  ptr data;
  usize top;
  usize capacity;

  /**
   * Check whether the current allocation is enough, else try to alloc/realloc
   *  memory
   **/
  [[nodiscard]] inline error_code check_allocation() noexcept {
    if (this->data == nullptr) {
      return this->allocate(VECTOR_INITIAL_SIZE);
    }

    if (this->top == this->capacity) {
      return this->reallocate(this->capacity * VECTOR_K);
    }

    return error_code::OK;
  }

  [[nodiscard]] error_code allocate(usize new_size) noexcept {
    if constexpr (std::is_fundamental<T>::value) {
      // Primitive use calloc
      this->data = (ptr)std::calloc(new_size, sizeof(T)); // NOLINT
    } else {
      // Use memory store for classes
      this->data = new (std::nothrow) T[new_size]; // NOLINT
    }

    if (this->data == nullptr) {
      return error_code::BAD_ALLOC;
    }

    this->capacity = new_size;
    return error_code::OK;
  }

  [[nodiscard]] error_code reallocate(usize new_size) noexcept {
    ptr new_data = nullptr;
    if constexpr (std::is_fundamental<T>::value) {
      new_data = (ptr)std::realloc(this->data, new_size * sizeof(T)); // NOLINT
    } else {
      new_data = new (std::nothrow) T[new_size]; // NOLINT
    }

    if (new_data == nullptr) {
      return error_code::BAD_ALLOC;
    }

    if constexpr (std::is_fundamental<T>::value) {
      // For realloc, set the new data to 0
      std::memset(
          this->data + this->capacity, 0,
          (new_size - this->capacity) * sizeof(T)
      );
    } else {
      // For new store, need to transfer the data manually
      for (usize i = 0; i < this->top; ++i) {
        new_data[i] = std::move(this->data[i]);
      }

      delete[] this->data;
    }

    this->data = new_data;
    this->capacity = new_size;

    return error_code::OK;
  }
};

} // namespace ds

#endif
