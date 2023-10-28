/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-07
 *==========================*/

#include "catch2/catch_test_macros.hpp"
#include "core/draw/anim.hpp"
#include "core/draw/types.hpp"
#include "types.hpp"
#include <cstring>

const ivec size{2, 2};
const i32 isize = size.x * size.y;

using namespace draw;

void test_init_anim(const Anim& anim) noexcept {
  REQUIRE(anim.get_size().x == size.x);
  REQUIRE(anim.get_size().y == size.y);
  REQUIRE(anim.get_frame_count() == 1);
  REQUIRE(anim.get_layer_count() == 1);
}

TEST_CASE("Anim: init", "[draw]") {
  Anim anim{};

  SECTION("rgba8") {
    anim.init(size, ColorType::RGBA8);
    test_init_anim(anim);

    // Layer should be filled with 0
    auto layer = anim.get_layer(0, 0);
    REQUIRE(layer.get_size().x == size.x);
    REQUIRE(layer.get_size().y == size.y);

    u32 expected_ptr[] = {0U, 0U, 0U, 0U}; // All zeroes
    auto* ptr = layer.get_ptr();
    REQUIRE(std::memcmp(ptr, expected_ptr, isize * sizeof(rgba8)) == 0);
  }

  SECTION("rgba16") {
    anim.init(size, ColorType::RGBA16);
    test_init_anim(anim);

    // Layer should be filled with 0
    auto layer = anim.get_layer(0, 0);
    REQUIRE(layer.get_size().x == size.x);
    REQUIRE(layer.get_size().y == size.y);

    u64 expected_ptr[] = {0U, 0U, 0U, 0U}; // All zeroes
    auto* ptr = layer.get_ptr();
    REQUIRE(std::memcmp(ptr, expected_ptr, isize * sizeof(rgba16)) == 0);
  }
}

TEST_CASE("Anim: Layer Resizeable", "[draw]") {
  Anim anim{};
  anim.init(size, ColorType::RGBA8);

  rgba8 color1{0x11U, 0x11U, 0x11U, 0x11U};
  rgba8 color2{0x22U, 0x22U, 0x22U, 0x22U};
  u32 expected_ptr0[] = {0U, 0U, 0U, 0U};
  u32 expected_ptr1[] = {0x11111111U, 0x11111111U, 0x11111111U, 0x11111111U};
  u32 expected_ptr2[] = {0x22222222U, 0x22222222U, 0x22222222U, 0x22222222U};

  auto layer = anim.get_layer(0, 0);
  for (i32 i = 0; i < isize; ++i) {
    layer.paint(i, color1);
  }

  // Insert after the data
  anim.insert_layer(1);

  REQUIRE(anim.get_layer_count() == 2);
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, 0).get_ptr(), expected_ptr1, isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, 1).get_ptr(), expected_ptr0, isize * sizeof(rgba8)
      ) == 0
  );

  // Color the new inserted frame
  layer = anim.get_layer(0, 1);
  for (i32 i = 0; i < isize; ++i) {
    layer.paint(i, color2);
  }

  // Insert before the data
  anim.insert_layer(0);
  REQUIRE(anim.get_layer_count() == 3);
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, 0).get_ptr(), expected_ptr0, isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, 1).get_ptr(), expected_ptr1, isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, 2).get_ptr(), expected_ptr2, isize * sizeof(rgba8)
      ) == 0
  );

  // Try to resize the container and check if resize is correct
  auto org_layer_capacity = anim.get_layer_capacity();
  anim.insert_layers(1, anim.get_layer_capacity());
  REQUIRE(anim.get_layer_count() == org_layer_capacity + 3);
  for (i32 i = 0; i <= org_layer_capacity; ++i) {
    REQUIRE(
        std::memcmp(
            anim.get_layer(0, i).get_ptr(), expected_ptr0, isize * sizeof(rgba8)
        ) == 0
    );
  }
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, org_layer_capacity + 1).get_ptr(), expected_ptr1,
          isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, org_layer_capacity + 2).get_ptr(), expected_ptr2,
          isize * sizeof(rgba8)
      ) == 0
  );

  REQUIRE(anim.get_frame_count() == 1);
}

TEST_CASE("Anim: Frame Resizeable", "[draw]") {
  Anim anim{};
  anim.init(size, ColorType::RGBA8);

  rgba8 color1{0x11U, 0x11U, 0x11U, 0x11U};
  rgba8 color2{0x22U, 0x22U, 0x22U, 0x22U};
  u32 expected_ptr0[] = {0U, 0U, 0U, 0U};
  u32 expected_ptr1[] = {0x11111111U, 0x11111111U, 0x11111111U, 0x11111111U};
  u32 expected_ptr2[] = {0x22222222U, 0x22222222U, 0x22222222U, 0x22222222U};

  auto layer = anim.get_layer(0, 0);
  for (i32 i = 0; i < isize; ++i) {
    layer.paint(i, color1);
  }

  // Insert after the data
  anim.insert_frame(1);

  REQUIRE(anim.get_frame_count() == 2);
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, 0).get_ptr(), expected_ptr1, isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(1, 0).get_ptr(), expected_ptr0, isize * sizeof(rgba8)
      ) == 0
  );

  // Color the new inserted frame
  layer = anim.get_layer(1, 0);
  for (i32 i = 0; i < isize; ++i) {
    layer.paint(i, color2);
  }

  // Insert before the data
  anim.insert_frame(0);
  REQUIRE(anim.get_frame_count() == 3);
  REQUIRE(
      std::memcmp(
          anim.get_layer(0, 0).get_ptr(), expected_ptr0, isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(1, 0).get_ptr(), expected_ptr1, isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(2, 0).get_ptr(), expected_ptr2, isize * sizeof(rgba8)
      ) == 0
  );

  // Try to resize the container and check if resize is correct
  auto org_layer_capacity = anim.get_layer_capacity();
  anim.insert_frames(1, anim.get_layer_capacity());
  REQUIRE(anim.get_frame_count() == org_layer_capacity + 3);
  for (i32 i = 0; i <= org_layer_capacity; ++i) {
    REQUIRE(
        std::memcmp(
            anim.get_layer(i, 0).get_ptr(), expected_ptr0, isize * sizeof(rgba8)
        ) == 0
    );
  }
  REQUIRE(
      std::memcmp(
          anim.get_layer(org_layer_capacity + 1, 0).get_ptr(), expected_ptr1,
          isize * sizeof(rgba8)
      ) == 0
  );
  REQUIRE(
      std::memcmp(
          anim.get_layer(org_layer_capacity + 2, 0).get_ptr(), expected_ptr2,
          isize * sizeof(rgba8)
      ) == 0
  );

  REQUIRE(anim.get_layer_count() == 1);
}

