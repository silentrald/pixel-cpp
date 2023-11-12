/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#ifndef MODULES_HISTORY_SNAPSHOT_HPP
#define MODULES_HISTORY_SNAPSHOT_HPP

#include "core/draw/anim.hpp"
#include "model/model.hpp"
#include "types.hpp"

namespace history {

class Snapshot {
public:
  Snapshot(const Snapshot&) noexcept = delete;
  Snapshot& operator=(const Snapshot&) noexcept = delete;

  Snapshot() noexcept = default;
  Snapshot(Snapshot&& rhs) noexcept = default;
  Snapshot& operator=(Snapshot&& rhs) noexcept = default;
  ~Snapshot() noexcept = default;

  void snap_default(const Model& model) noexcept;
  void snap_anim(const Model& model) noexcept;
  void restore(Model& model) const noexcept;

  void reset() noexcept;

private:
  draw::Anim anim{};
  i32 frame_id = -1;
  i32 layer_index = -1;
};

} // namespace history

#endif

