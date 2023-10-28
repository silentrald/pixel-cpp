/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-05
 *==========================*/

#ifndef PXL_DRAW_ANIM_HPP
#define PXL_DRAW_ANIM_HPP

#include "./frame.hpp"
#include "./types.hpp"
#include "types.hpp"
#include <cassert>

namespace draw {

// NOTE: have a file cache in the future
/**
 * Contains the animation data
 **/
class Anim {
public:
  Anim() noexcept = default;
  Anim(const Anim&) noexcept = delete;
  Anim& operator=(const Anim&) noexcept = delete;
  Anim(Anim&& rhs) noexcept;
  Anim& operator=(Anim&& rhs) noexcept;

  ~Anim() noexcept;

  void init(ivec size, ColorType type) noexcept;
  void copy(const Anim& other) noexcept;

  [[nodiscard]] data_ptr get_ptr() noexcept;
  [[nodiscard]] ColorType get_type() const noexcept;
  [[nodiscard]] i32 get_frame_count() const noexcept;
  [[nodiscard]] i32 get_layer_count() const noexcept;
  [[nodiscard]] i32 get_frame_capacity() const noexcept;
  [[nodiscard]] i32 get_layer_capacity() const noexcept;
  [[nodiscard]] ivec get_size() const noexcept;
  [[nodiscard]] i32 get_width() const noexcept;
  [[nodiscard]] i32 get_height() const noexcept;

  /**
   * Check if the pixel position is within the dimension
   **/
  [[nodiscard]] bool has_point(ivec point) const noexcept;

  void clear() noexcept;

  void insert_frame(i32 index) noexcept;
  void insert_layer(i32 index) noexcept;

  // Inserts (count) number of frames at the index
  void insert_frames(i32 index, i32 count) noexcept;
  // Inserts (count) number of layers at the index
  void insert_layers(i32 index, i32 count) noexcept;

  [[nodiscard]] Frame get_frame(i32 index) noexcept {
    assert(index >= 0 && index < this->frame_count);

    return Frame{// NOLINTNEXTLINE
                 this->ptr + index * this->next_frame, this->next_layer,
                 this->size, this->type};
  }

  [[nodiscard]] Layer get_layer(i32 frame, i32 layer) noexcept {
    assert(frame >= 0 && frame < this->frame_count);
    assert(layer >= 0 && layer < this->layer_count);

    return Layer{
        // NOLINTNEXTLINE
        this->ptr + frame * this->next_frame + layer * this->next_layer,
        this->size, this->type};
  }

  // === Debugging === //
  void print() const noexcept;

private:
  // NOTE: check if there is a need for multiple data pointers
  data_ptr ptr = nullptr;
  i32 max_size = 0;
  // how many frames can fit in the total allocated memory
  i32 frame_capacity = 0;
  i32 frame_count = 0;
  // what to add to get the next frame data
  i32 next_frame = 0;
  // how many layers can fit in the total allocated memory
  i32 layer_capacity = 0;
  i32 layer_count = 0;
  // what to add to get the next layer data
  i32 next_layer = 0;
  ColorType type = ColorType::NONE;
  ivec size{};

  [[nodiscard]] i32 get_datatype_size() const noexcept;

  // === Resize Logic === //
  // TODO: May error, bad alloc
  void resize_frame(i32 new_frame_capacity) noexcept;
  // TODO: May error, bad alloc
  void resize_layer(i32 new_layer_capacity) noexcept;
};

} // namespace draw

#endif

