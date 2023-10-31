/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-07
 *==========================*/

#include "catch2/catch_test_macros.hpp"
#include "core/draw/types.hpp"
#include "core/logger/logger.hpp"
#include "types.hpp"
#include <cstring>
#include <utility>

/* #define private public */
#include "core/draw/anim.hpp"

inline const ivec SIZE{2, 2};
inline const i32 ISIZE = SIZE.x * SIZE.y;

using namespace draw;

void test_color(rgba8 c1, rgba8 c2) {
  REQUIRE(std::memcmp(&c1, &c2, 4) == 0);
}

void test_empty_layer(const Layer& layer, u32 id) noexcept {
  REQUIRE(layer.get_id() == id);
  REQUIRE(layer.get_size().x == SIZE.x);
  REQUIRE(layer.get_width() == SIZE.x);
  REQUIRE(layer.get_size().y == SIZE.y);
  REQUIRE(layer.get_height() == SIZE.y);
  REQUIRE(layer.get_ptr() != nullptr);

  for (i32 i = 0; i < ISIZE; ++i) {
    test_color(*(rgba8*)layer.get_pixel(i), {});
  }
}

void test_init_anim(const Anim& anim) noexcept {
  REQUIRE(anim.get_size().x == SIZE.x);
  REQUIRE(anim.get_width() == SIZE.x);
  REQUIRE(anim.get_size().y == SIZE.y);
  REQUIRE(anim.get_height() == SIZE.y);
  REQUIRE(anim.get_layer_count() == 1);

  auto layer = anim.get_layer(1U);
  test_empty_layer(layer, 1U);
}

TEST_CASE("Anim: layers modification", "[draw]") {
  Anim anim{};

  anim.init(SIZE, ColorType::RGBA8);
  test_init_anim(anim);

  u32 id = 0U;
  Layer layer{};

  for (i32 i = 0; i < 5; ++i) {
    id = anim.insert_layer(0);
    REQUIRE(id != 1U);
    REQUIRE(anim.get_layer_count() == i + 2);

    layer = std::move(anim.get_layer(id));
    test_empty_layer(layer, id);
  }

  // Check ordering
  for (i32 i = 0; i < 6; ++i) {
    id = anim.get_layer_id(1, i);
    REQUIRE(i + id == 6U);
  }

  // Edit the layers
  const rgba8 expected_colors[] = {
      {0x11, 0x11, 0x11, 0x11}, {0x22, 0x22, 0x22, 0x22},
      {0x33, 0x33, 0x33, 0x33}, {0x44, 0x44, 0x44, 0x44},
      {0x55, 0x55, 0x55, 0x55}, {0x66, 0x66, 0x66, 0x66}};
  for (id = 1U; id <= 6U; ++id) {
    layer = std::move(anim.get_layer(id));
    for (i32 i = 0; i < ISIZE; ++i) {
      layer.paint(i, expected_colors[id - 1]);
    }
  }

  for (id = 1U; id <= 6U; ++id) {
    layer = std::move(anim.get_layer(id));
    for (i32 i = 0; i < ISIZE; ++i) {
      test_color(*(rgba8*)layer.get_pixel(i), expected_colors[id - 1]);
    }
  }
}

