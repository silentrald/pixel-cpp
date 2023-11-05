/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-30
 *===============================*/

#ifndef PXL_FILE_PNG_HPP
#define PXL_FILE_PNG_HPP

#include "core/draw/anim.hpp"
#include "types.hpp"

namespace file {

/**
 * Handles importing/exporting PNG files
 **/
class Png {
public:
  void export_frame(const draw::Anim& anim, u32 frame_id, const c8* path)
      const noexcept;

private:
};

} // namespace file

#endif

