/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-29
 *==========================*/

#ifndef PXL_DRAW_ANIM_HPP
#define PXL_DRAW_ANIM_HPP

#include "./layer.hpp"
#include "./types.hpp"
#include "core/ds/vector.hpp"
#include "types.hpp"

namespace draw {

/**
 * Animation data containing the image information for frames/layers
 **/
class Anim {
public:
  Anim() noexcept = default;
  Anim(const Anim&) noexcept = delete;
  Anim& operator=(const Anim&) noexcept = delete;
  Anim(Anim&& rhs) noexcept = default;
  Anim& operator=(Anim&& rhs) noexcept = default;
  ~Anim() noexcept = default;

  void init(ivec size, ColorType type) noexcept;
  /* void load(const c8* str) noexcept; // For file loading */

  void copy(const Anim& other) noexcept;
  void minicopy(const Anim& other) noexcept;
  void clear() noexcept;

  [[nodiscard]] bool has_point(ivec pos) const noexcept;

  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;

  [[nodiscard]] i32 get_layer_count() const noexcept;

  [[nodiscard]] u32 insert_layer(i32 index) noexcept;
  [[nodiscard]] Layer get_layer(u32 id) const noexcept;
  [[nodiscard]] u32 get_layer_id(i32 frame_id, i32 layer_index) const noexcept;

  [[nodiscard]] explicit operator bool() const noexcept;

private:
  // SOA Arena of layers
  class LayerArena {
  public:
    LayerArena() noexcept = default;
    LayerArena(const LayerArena&) noexcept = delete;
    LayerArena& operator=(const LayerArena&) noexcept = delete;

    void init(i32 bytes, i32 capacity) noexcept;
    LayerArena(LayerArena&& rhs) noexcept;
    LayerArena& operator=(LayerArena&& rhs) noexcept;
    void copy(const LayerArena& other) noexcept;
    void minicopy(const LayerArena& other) noexcept;
    ~LayerArena() noexcept;

    [[nodiscard]] data_ptr get_ptr() const noexcept;
    [[nodiscard]] data_ptr get_layer(u32 id) const noexcept;

    void create_layer(u32 new_id) noexcept;

  private:
    // TODO: Change datatypes (64 bit) to support higher sizes
    data_ptr ptr = nullptr;
    i32 insert = 0;
    i32 last = 0;
    i32 capacity = 0;
    i32 size = 0;

    i32 bytes = 0;
    i32 allocated_size = 0;

    // === Copy Helpers === //
    void copy_empty(const LayerArena& other) noexcept;
    void copy_normal(const LayerArena& other) noexcept;
    void copy_overfit(const LayerArena& other) noexcept;
    void copy_grow(const LayerArena& other) noexcept;

    // === Memory === //
    void allocate(i32 allocated_size) noexcept;
    void reallocate(i32 new_allocated_size) noexcept;
  };

  // SOA Arena of frames
  class FrameArena {
  public:
    FrameArena() noexcept = default;
    FrameArena(const FrameArena&) noexcept = delete;
    FrameArena& operator=(const FrameArena&) noexcept = delete;

    void init() noexcept;
    FrameArena(FrameArena&& rhs) noexcept;
    FrameArena& operator=(FrameArena&& rhs) noexcept;
    void copy(const FrameArena& other) noexcept;
    // NOTE: Frame can't be resized
    /* void minicopy(const FrameArena& other) noexcept; */
    ~FrameArena() noexcept;

    [[nodiscard]] i32 get_layer_count() const noexcept;
    [[nodiscard]] i32 get_layer_capacity() const noexcept;
    [[nodiscard]] u32
    get_layer_id(i32 frame_id, i32 layer_index) const noexcept;
    [[nodiscard]] i32 get_allocated_size() const noexcept;

    void insert_layer(i32 index, u32 layer_id) noexcept;

  private:
    u32* ptr = nullptr;
    i32 layer_count = 0;
    i32 frame_count = 0;
    i32 layer_capacity = 0;
    i32 frame_capacity = 0;
    i32 allocated_size = 0;
    // NOTE: Padded by +4

    // === Copy Helpers === //
    void copy_empty(const FrameArena& other) noexcept;
    void copy_normal(const FrameArena& other) noexcept;
    void copy_overfit(const FrameArena& other) noexcept;
    void copy_grow(const FrameArena& other) noexcept;

    // === Memory === //
    void allocate(i32 new_size) noexcept;
    void reallocate_on_layer(i32 new_layer_capacity) noexcept;
  };

  LayerArena layers{};
  FrameArena frames{};

  u32 layers_id = 0U;

  // Dimension size of the animation
  ivec size{};

  // What color depth is being used
  ColorType type = ColorType::NONE;
};

} // namespace draw

#endif

