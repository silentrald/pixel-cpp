/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-05
 *===============================*/

#ifndef PXL_FILE_PXL_HPP
#define PXL_FILE_PXL_HPP

#include "core/draw/anim.hpp"
#include "types.hpp"

namespace file {

class Pxl {
public:
  void save(const draw::Anim& anim, const c8* path) const noexcept;
  /* draw::Anim load(const c8* path) const noexcept; */

private:
};

} // namespace file

#endif

