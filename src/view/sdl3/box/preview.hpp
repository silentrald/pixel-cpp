/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-12-10
 *==========================*/

#ifndef PXL_VIEW_SDL3_PREVIEW_BOX_HPP
#define PXL_VIEW_SDL3_PREVIEW_BOX_HPP

#include "../input/button.hpp"
#include "../texture.hpp"
#include "./box.hpp"
#include "core/draw/anim.hpp"
#include "core/draw/types.hpp"
#include "core/ds/vector.hpp"

namespace view::sdl3::widget {

class PreviewBox final : public Box {
public:
  PreviewBox() noexcept = default;
  PreviewBox(const PreviewBox&) noexcept = delete;
  PreviewBox& operator=(const PreviewBox&) noexcept = delete;
  PreviewBox(PreviewBox&&) noexcept = default;
  ~PreviewBox() noexcept override = default;
  PreviewBox& operator=(PreviewBox&&) noexcept = default;

  void init() noexcept;
  [[nodiscard]] error_code set_anim(const draw::Anim* anim) noexcept;
  void set_active_frame(usize active_frame) noexcept;
  void set_playing(bool playing) noexcept;

  void resize(const frect& rect) noexcept override;
  void reset() noexcept override;
  void locale_updated() noexcept override;
  void input(const event::Input& evt, InputData& data) noexcept override;
  void update(f32 delta) noexcept override;
  void render() noexcept override;

private:
  const draw::Anim* anim = nullptr;
  ds::vector<draw::data_type> pixels{};
  Texture tex{};

public:
  frect texture_rect{};

private:
  widget::Button play_btn{};
  usize active_frame = 0U;
  usize cursor = 0U;
  f32 fps = 12.0F;
  f32 spf = 1.0F / 12.0F;
  f32 ticks = 0.0F;

  bool playing = false;
  bool loop = true;

  void set_frame_texture(usize frame_index) noexcept;
};

} // namespace view::sdl3::widget

#endif

