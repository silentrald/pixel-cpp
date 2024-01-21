/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-01-22
 *==========================*/

#include "core/ds/hashmap.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"
#include "catch2/catch_test_macros.hpp"
#include "core/logger/logger.hpp"
#include "types.hpp"
#include <cstdlib>
#include <stdexcept>
#include <unordered_map>

struct NoHash {
  usize operator()(const i32& val) const noexcept {
    return val;
  }
};

TEST_CASE("hashmap: init", "[ds]") {
  ds::hashmap<i32, i32, NoHash> map{};
  REQUIRE(map.get_size() == 0);

  for (usize i = 0; i < 25; ++i) {
    TRY_ABORT(map.insert(i, i * 2), "Insert error");
  }
  REQUIRE(map.get_size() == 25);

  i32* ptr = nullptr;
  for (usize i = 0; i < 25; ++i) {
    ptr = map[i];
    REQUIRE(ptr != nullptr);
    REQUIRE(*ptr == i * 2);
  }
  REQUIRE(map[25] == nullptr);

  for (usize i = 0; i < 12; ++i) {
    TRY_ABORT(map.insert(i, i * 3), "Insert error");
  }

  for (usize i = 8; i < 16; ++i) {
    map.remove(i);
  }
  REQUIRE(map.get_size() == 17);

  for (usize i = 0; i < 25; ++i) {
    ptr = map[i];
    if (i < 8) {
      REQUIRE(ptr != nullptr);
      REQUIRE(*ptr == i * 3);
    } else if (i < 16) {
      REQUIRE(ptr == nullptr);
    } else {
      REQUIRE(ptr != nullptr);
      REQUIRE(*ptr == i * 2);
    }
  }
}

TEST_CASE("benchmarks", "[ds]") {
  const usize ARRAY_SIZE = 100000U;
  const usize MAX_INT = 10000000U;
  usize array[ARRAY_SIZE];
  for (auto i = 0; i < ARRAY_SIZE; ++i) {
    array[i] = rand() % MAX_INT;
  }

  BENCHMARK("ds insert") {
    ds::hashmap<i32, i32, NoHash> map{};
    for (usize i = 0; i < ARRAY_SIZE; ++i) {
      if (map.insert(array[i], i) != error_code::OK) {
        throw std::runtime_error("Error");
      }
    }
  };

  BENCHMARK("std insert") {
    std::unordered_map<i32, i32, NoHash> map{};
    for (usize i = 0; i < ARRAY_SIZE; ++i) {
      map.insert({array[i], i});
    }
  };

  ds::hashmap<i32, i32, NoHash> ds_map{};
  for (usize i = 0; i < ARRAY_SIZE; ++i) {
    if (ds_map.insert(array[i], i) != error_code::OK) {
      throw std::runtime_error("Error");
    }
  }
  BENCHMARK("ds access") {
    for (usize i = 0; i < ARRAY_SIZE; ++i) {
      (void)ds_map[array[i]];
    }
  };

  std::unordered_map<i32, i32, NoHash> std_map{};
  for (usize i = 0; i < ARRAY_SIZE; ++i) {
    std_map.insert({array[i], i});
  }
  BENCHMARK("std access") {
    for (usize i = 0; i < ARRAY_SIZE; ++i) {
      std_map[array[i]];
    }
  };
}
