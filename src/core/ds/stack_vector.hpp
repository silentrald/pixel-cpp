/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-30
 *===============================*/

#ifndef PXL_DS_STACK_VECTOR_HPP
#define PXL_DS_STACK_VECTOR_HPP

#include "types.hpp"
#include <cassert>
#include <utility>

namespace ds {

/**
 * Stack allocated vector, when capacity is already known on compile time
 **/
template <typename T, usize Capacity> class stack_vector {
private:
  using val = T;
  using ptr = T*;
  using ref = T&;
  using cref = const T&;
  using rref = T&&;

public:
  stack_vector(const stack_vector&) noexcept = delete;
  stack_vector& operator=(const stack_vector&) noexcept = delete;

  stack_vector() noexcept : array(), top(0) {}

  ~stack_vector() noexcept = default;

  stack_vector(stack_vector&& rhs) noexcept : top(rhs.top) {
    for (usize i = 0; i < this->top; ++i) {
      this->array[i] = std::move(rhs.array[i]);
    }
  }

  stack_vector& operator=(stack_vector&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->top = rhs.top;
    for (usize i = 0; i < this->top; ++i) {
      this->array[i] = std::move(rhs.array[i]);
    }

    rhs.array = nullptr;

    return *this;
  }

  // === Info === //

  [[nodiscard]] ptr get_array() const noexcept {
    return this->array;
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
   * Check if the vector has reached capacity
   **/
  [[nodiscard]] bool is_full() const noexcept {
    return this->top == Capacity;
  }

  /**
   * The maximum number of elements in the vector, this can change if a vector
   *  mutation happens
   **/
  [[nodiscard]] usize get_capacity() const noexcept {
    return Capacity;
  }

  // === Modifiers === //

  void push_back(cref elem) noexcept {
    assert(this->top < Capacity);

    this->array[this->top] = elem;
    ++this->top;
  }

  void push_back(rref elem) noexcept {
    assert(this->top < Capacity);

    this->array[this->top] = std::move(elem);
    ++this->top;
  }

  rref pop() noexcept {
    assert(this->top > 0);

    --this->top;
    return std::move(this->array[this->top + 1]);
  }

  void insert(usize index, rref elem) noexcept {
    assert(index >= 0 && index <= this->top);

    // Shift
    for (usize i = this->top; i > index; --i) {
      this->array[i] = std::move(this->array[i - 1]);
    }
    this->array[index] = std::move(elem);
    ++this->top;
  }

  void remove(usize index) noexcept {
    assert(this->top > 0);

    for (usize i = index + 1; i < this->top; ++i) {
      this->array[i - 1] = std::move(this->data[i]);
    }
    --this->top;
  }

  void clear() noexcept {
    // TODO: Check this if clearing the data is needed
    this->top = 0;
  }

  // === Accessing === //

  [[nodiscard]] ref operator[](usize index) noexcept {
    assert(index >= 0 && index < this->top);
    return this->array[index];
  }

  [[nodiscard]] cref operator[](usize index) const noexcept {
    assert(index >= 0 && index < this->top);
    return this->array[index];
  }

  [[nodiscard]] ref back() noexcept {
    assert(this->top > 0);
    return this->array[this->top - 1];
  }

  [[nodiscard]] cref back() const noexcept {
    assert(this->top > 0);
    return this->array[this->top - 1];
  }

  [[nodiscard]] ref front() noexcept {
    assert(this->top > 0);
    return *this->array;
  }

  [[nodiscard]] cref front() const noexcept {
    assert(this->top > 0);
    return *this->array;
  }

private:
  val array[Capacity]; // NOLINT
  usize top;
};

} // namespace ds

#endif
