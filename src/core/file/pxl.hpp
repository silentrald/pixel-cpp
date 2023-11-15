/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-05
 *===============================*/

#ifndef PXL_FILE_PXL_HPP
#define PXL_FILE_PXL_HPP

#include "core/draw/anim.hpp"
#include "core/draw/types.hpp"
#include "core/ds/vector.hpp"
#include "types.hpp"

namespace file {

class Pxl {
public:
  void set_auto_save(u32 auto_save) noexcept;

  /**
   * Check if pxl will auto-save. If this returns true, call force_auto_save.
   **/
  [[nodiscard]] bool will_auto_save() noexcept;

  [[nodiscard]] error_code force_auto_save(const draw::Anim& anim) noexcept;

  /**
   * Tries to auto save. This will only save if auto_save is greater than 0U and
   *  the number of actions is the same with the auto_save value. This does not
   *  work if auto_save is set to 0.
   **/
  [[nodiscard]] error_code try_auto_save(const draw::Anim& anim) noexcept;

  // TODO: Remove path param, get path from anim.name
  [[nodiscard]] error_code
  save(const draw::Anim& anim, const c8* path) noexcept;
  [[nodiscard]] expected<draw::Anim> load(const c8* path) noexcept;

private:
  // If this is set to 0U, autosave is disabled
  u32 auto_save = 0U;
  u32 actions = 0U;

  ds::vector<draw::data_type> pixels{};
};

} // namespace file

#endif

