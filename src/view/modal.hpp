/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-28
 *==========================*/

#ifndef PXL_VIEW_MODAL_HPP
#define PXL_VIEW_MODAL_HPP

#include "types.hpp"

namespace view::modal {

enum class Id {
  NONE,
  NEW_FILE_MODAL,
};

struct NewFileData {
  ivec size{};
};

} // namespace view::modal

#endif

