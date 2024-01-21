/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-01-22
 *==========================*/

#ifndef PXL_DS_HASHMAP_HPP
#define PXL_DS_HASHMAP_HPP

#include "types.hpp"
#include <functional>
#include <new>
#include <utility>

namespace ds {

inline const usize HASHMAP_INITIAL_SIZE = 11;
inline const f32 HASHMAP_LOAD_FACTOR = 0.9F;

/**
 * Robin hood hash table with backward shift delete implementation
 * NOTE: Only works for primitive datatypes
 **/
template <
    typename Key, typename Value, typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
class hashmap {
public:
  hashmap() noexcept : bucket(nullptr), size(0), max_size(0), capacity(0) {}

  hashmap(const hashmap&) = delete;
  hashmap& operator=(const hashmap&) = delete;

  hashmap(hashmap&& other) noexcept
      : bucket(other.bucket),
        size(other.size),
        max_size(other.max_size),
        capacity(other.capacity) {
    other.bucket = nullptr;
  }

  hashmap& operator=(hashmap&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->bucket = rhs.bucket;
    this->size = rhs.size;
    this->max_size = rhs.max_size;
    this->capacity = rhs.capacity;
    rhs.bucket = nullptr;

    return *this;
  }

  ~hashmap() noexcept {
    if (this->bucket) {
      delete[] this->bucket; // NOLINT
      this->bucket = nullptr;
    }
  }

  // === Modifiers === //

  [[nodiscard]] error_code insert(Key&& key, Value&& val) noexcept {
    TRY(this->check_allocation());
    this->insert_node(
        Node{.key = std::move(key), .val = std::move(val), .empty = false}
    );
    return error_code::OK;
  }

  [[nodiscard]] error_code insert(const Key& key, Value&& val) noexcept {
    TRY(this->check_allocation());
    this->insert_node(Node{.key = key, .val = std::move(val), .empty = false});
    return error_code::OK;
  }

  [[nodiscard]] error_code insert(Key&& key, const Value& val) noexcept {
    TRY(this->check_allocation());
    this->insert_node(Node{.key = std::move(key), .val = val, .empty = false});
    return error_code::OK;
  }

  [[nodiscard]] error_code insert(const Key& key, const Value& val) noexcept {
    TRY(this->check_allocation());
    this->insert_node(Node{.key = key, .val = val, .empty = false});
    return error_code::OK;
  }

  void remove(const Key& key) noexcept {
    auto pos = this->get_pos(key);
    auto end = pos;

    while (true) {
      if (this->bucket[pos].empty) {
        return;
      }

      if (KeyEqual()(key, this->bucket[pos].key)) {
        this->bucket[pos].empty = true;
        --this->size;
        break;
      }

      ++pos;
      if (pos >= this->capacity) {
        pos = 0;
      }

      if (pos == end) {
        return;
      }
    }

    // Backward shift delete algorithm
    for (pos = pos + 1U; pos < this->capacity; ++pos) {
      if (this->bucket[pos].empty || this->bucket[pos].distance == 0U) {
        return;
      }
      --this->bucket[pos].distance;
      this->bucket[pos - 1U] = std::move(this->bucket[pos]);
    }

    if (this->bucket[0].empty || this->bucket[0].distance == 0U) {
      return;
    }
    --this->bucket[0].distance;
    this->bucket[this->capacity - 1U] = std::move(this->bucket[0]);

    for (pos = 1U; pos < end; ++pos) {
      if (this->bucket[pos].empty || this->bucket[pos].distance == 0U) {
        return;
      }
      --this->bucket[pos].distance;
      this->bucket[pos - 1U] = std::move(this->bucket[pos]);
    }
  }

  void clear() noexcept {
    if (this->bucket) {
      delete[] this->bucket; // NOLINT
      this->bucket = nullptr;
    }
    this->size = this->max_size = this->capacity = 0U;
  }

  // === Accessors === //

  [[nodiscard]] Value* operator[](const Key& key) noexcept {
    return this->find(key);
  }

  [[nodiscard]] const Value* operator[](const Key& key) const noexcept {
    return this->find(key);
  }

  [[nodiscard]] usize get_size() const noexcept {
    return this->size;
  }

private:
  struct Node {
    Key key;
    Value val;
    usize distance = 0U;
    bool empty = true;
  };

  Node* bucket;
  usize size;
  usize max_size;
  usize capacity;

  // === Helpers === //

  inline usize get_pos(const Key& key) const noexcept {
    return Hash()(key) % this->capacity;
  }

  void insert_node(Node&& node) noexcept {
    auto pos = this->get_pos(node.key);
    auto end = pos;

    while (true) {
      if (this->bucket[pos].empty) {
        this->bucket[pos] = std::move(node);
        ++this->size;
        break;
      }

      if (KeyEqual()(node.key, this->bucket[pos].key)) {
        this->bucket[pos].val = std::move(node.val);
        break;
      }

      ++pos;
      if (pos >= this->capacity) {
        pos = 0;
      }

      if (pos == end) {
        break;
      }
    }
  }

  [[nodiscard]] Value* find(const Key& key) const noexcept {
    auto pos = this->get_pos(key);
    auto end = pos;

    while (true) {
      if (this->bucket[pos].empty) {
        return nullptr;
      }

      if (KeyEqual()(key, this->bucket[pos].key)) {
        return &this->bucket[pos].val;
      }

      ++pos;
      if (pos >= this->capacity) {
        pos = 0;
      }

      if (pos == end) {
        return nullptr;
      }
    }
  }

  // === Memory === //

  [[nodiscard]] error_code check_allocation() noexcept {
    if (this->bucket == nullptr) {
      return this->allocate(HASHMAP_INITIAL_SIZE);
    }

    if (this->size >= this->max_size) {
      return this->reallocate(this->capacity * 2U + 1U);
    }

    return error_code::OK;
  }

  [[nodiscard]] error_code allocate(usize new_capacity) noexcept {
    this->bucket = new (std::nothrow) Node[new_capacity]; // NOLINT
    if (this->bucket == nullptr) {
      return error_code::BAD_ALLOC;
    }

    this->capacity = new_capacity;
    this->max_size = new_capacity * HASHMAP_LOAD_FACTOR;

    return error_code::OK;
  }

  [[nodiscard]] error_code reallocate(usize new_capacity) noexcept {
    auto* new_bucket = new (std::nothrow) Node[new_capacity]; // NOLINT
    if (new_bucket == nullptr) {
      return error_code::BAD_ALLOC;
    }

    // Swap the pointers and capacity
    std::swap(this->bucket, new_bucket);
    std::swap(this->capacity, new_capacity);
    this->size = 0;

    // Transfer the old bucket to the new bucket
    for (usize i = 0; i < new_capacity; ++i) {
      if (new_bucket[i].empty) {
        continue;
      }

      new_bucket[i].distance = 0;
      this->insert_node(std::move(new_bucket[i]));
    }

    this->max_size = this->capacity * HASHMAP_LOAD_FACTOR;

    return error_code::OK;
  }
};

} // namespace ds

#endif
