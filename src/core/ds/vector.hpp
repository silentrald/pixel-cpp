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
#include <cstdlib>
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

namespace ds {

inline const i32 VECTOR_INITIAL_SIZE = 4;
inline const i32 VECTOR_K = 2;

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

  vector() noexcept = default;

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

  /**
   * Number of elements within the vector
   **/
  [[nodiscard]] i32 get_size() const noexcept {
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
  [[nodiscard]] i32 get_capacity() const noexcept {
    return this->capacity;
  }

  // === Modifiers === //

  // TODO: Replace to std::expected
  void resize(i32 new_size) noexcept {
    assert(new_size >= 0);

    if (this->data) {
      this->reallocate(new_size);
    } else {
      this->allocate(new_size);
    }
    this->top = new_size;
  }

  // TODO: Replace to std::expected
  void push_back(rref elem) noexcept {
    this->check_allocation();

    this->data[this->top] = std::move(elem);
    ++this->top;
  }

  rref pop() noexcept {
    assert(this->top > 0);

    --this->top;
    return std::move(this->data[this->top + 1]);
  }

  // TODO: Replace to std::expected
  void insert(i32 index, rref elem) noexcept {
    this->check_allocation();

    // Shift
    for (i32 i = this->top; i > index; --i) {
      this->data[i] = std::move(data[i - 1]);
    }
    this->data[index] = std::move(elem);
    ++this->top;
  }

  void remove(i32 index) noexcept {
    assert(this->top > 0);

    for (i32 i = index + 1; i < this->top; ++i) {
      this->data[i - 1] = std::move(this->data[i]);
    }
    --this->top;
  }

  void clear() noexcept {
    // TODO: Check this if this is really needed
    this->top = 0;
  }

  // === Accessing === //

  [[nodiscard]] ref operator[](i32 index) noexcept {
    assert(index >= 0 && index < this->top);
    return this->data[index];
  }

  [[nodiscard]] cref operator[](i32 index) const noexcept {
    assert(index >= 0 && index < this->top);
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
  ptr data = nullptr;
  i32 top = 0;
  i32 capacity = 0;

  /**
   * Check whether the current allocation is enough, else try to alloc/realloc
   *  memory
   **/
  // TODO: Replace to std::expected
  inline void check_allocation() noexcept {
    if (this->data == nullptr) {
      this->allocate(VECTOR_INITIAL_SIZE);
    } else if (this->top == this->capacity) {
      this->reallocate(this->capacity * VECTOR_K);
    }
  }

  // TODO: Replace to std::expected
  void allocate(i32 new_size) noexcept {
    if constexpr (std::is_fundamental<T>::value) {
      // Primitive use malloc
      this->data = (ptr)std::malloc(new_size * sizeof(T)); // NOLINT
    } else {
      // Use memory store for classes
      this->data = new (std::nothrow) T[new_size]; // NOLINT
    }

    if (this->data == nullptr) {
      std::abort();
    }
    this->capacity = new_size;
  }

  // TODO: Replace to std::expected
  void reallocate(i32 new_size) noexcept {
    ptr new_data = nullptr;
    if constexpr (std::is_fundamental<T>::value) {
      new_data = (ptr)std::realloc(this->data, new_size * sizeof(T)); // NOLINT
    } else {
      new_data = new (std::nothrow) T[new_size]; // NOLINT
    }

    if (new_data == nullptr) {
      std::abort();
    }

    if constexpr (std::is_fundamental<T>::value) {
      // For realloc, set the new data to 0
      std::memset(this->data + this->capacity, 0, new_size - this->capacity);
    } else {
      // For new store, need to transfer the data manually
      for (i32 i = 0; i < this->top; ++i) {
        new_data[i] = std::move(this->data[i]);
      }

      delete[] this->data;
    }

    this->data = new_data;
    this->capacity = new_size;
  }
};

} // namespace ds

#endif

