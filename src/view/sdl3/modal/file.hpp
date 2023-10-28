/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#ifndef PXL_VIEW_SDL3_FILE_MODAL_HPP
#define PXL_VIEW_SDL3_FILE_MODAL_HPP

#include "../font.hpp"
#include "../texture.hpp"
#include "../widget/button.hpp"
#include "./modal.hpp"

namespace view::sdl3::widget {

class FileModal : public Modal {
public:
  FileModal() noexcept {
    this->id = modal::Id::NEW_FILE_MODAL;
  }

  FileModal(const FileModal&) noexcept = delete;
  FileModal& operator=(const FileModal&) noexcept = delete;
  FileModal(FileModal&&) noexcept = default;
  FileModal& operator=(FileModal&&) noexcept = default;
  ~FileModal() noexcept override = default;

  void init(const Renderer& renderer, const Font& font) noexcept;

  [[nodiscard]] void* get_data() const noexcept override;

  void resize(const frect& rect) noexcept override;
  void drag(fvec pos) noexcept override;
  void reset() noexcept override;
  void input(const event::Input& evt) noexcept override;
  void update() noexcept override;
  void render(const Renderer& renderer) const noexcept override;

private:
  // === Texts === //
  Texture new_file_tex{};
  frect new_file_rect{};

  Texture width_tex{};
  frect width_rect{};

  Texture height_tex{};
  frect height_rect{};

  Texture px_tex{};
  frect px_rect1{};
  frect px_rect2{};

  // === Buttons === //
  Button new_btn{};
  Button cancel_btn{};
};

} // namespace view::sdl3::widget

#endif

