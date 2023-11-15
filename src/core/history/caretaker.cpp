/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#include "./caretaker.hpp"
#include "core/logger/logger.hpp"
#include "types.hpp"
#include <utility>

namespace history {

error_code Caretaker::init(i32 capacity, const Model& model) noexcept {
  TRY(this->snapshots.resize(capacity));

  history::Snapshot snapshot{};
  snapshot.snap_default(model);
  snapshot.snap_anim(model);

  this->clear();
  this->snapshots[0] = std::move(snapshot);
  this->cursor = this->start_cursor = this->end_cursor = 0;

  return error_code::OK;
}

void Caretaker::snap_model(const Model& model) noexcept {
  history::Snapshot snapshot{};

  // TODO: Optimize logic here, for compact memory storage
  snapshot.snap_default(model);
  snapshot.snap_anim(model);

  this->push_snapshot(std::move(snapshot));
}

void Caretaker::push_snapshot(Snapshot&& snapshot) noexcept {
  this->remove_future_snapshots();

  // Move the cursor and move the new snapshot
  ++this->cursor;
  if (this->cursor == this->snapshots.get_size()) {
    this->cursor = 0;
  }
  this->snapshots[this->cursor] = std::move(snapshot);

  this->update_cursors();
}

void Caretaker::clear() noexcept {
  for (i32 i = 0; i < this->snapshots.get_size(); ++i) {
    this->snapshots[i].reset();
  }
}

void Caretaker::remove_future_snapshots() noexcept {
  if (this->cursor == this->end_cursor) {
    return;
  }

  if (this->cursor < this->end_cursor) {
    // +2 because +1 will contain the new snapshot
    for (i32 i = this->cursor + 2; i <= this->end_cursor; ++i) {
      this->snapshots[i].reset();
    }
    return;
  }

  for (i32 i = this->cursor + 2; i < this->snapshots.get_size(); ++i) {
    this->snapshots[i].reset();
  }

  for (i32 i = 0; i <= this->end_cursor; ++i) {
    this->snapshots[i].reset();
  }
}

void Caretaker::update_cursors() noexcept {
  this->end_cursor = this->cursor;

  if (this->start_cursor == -1) {
    this->start_cursor = 0;
  } else if (this->start_cursor == this->cursor) {
    ++this->start_cursor;
  }
}

bool Caretaker::can_undo() const noexcept {
  return this->cursor != this->start_cursor;
}

bool Caretaker::undo(Model& model) noexcept {
  if (!this->can_undo()) {
    return false;
  }
  if (this->cursor == 0) {
    this->cursor = this->snapshots.get_size() - 1;
  } else {
    --this->cursor;
  }

  this->snapshots[this->cursor].restore(model);

  return true;
}

bool Caretaker::can_redo() const noexcept {
  return this->cursor != this->end_cursor;
}

bool Caretaker::redo(Model& model) noexcept {
  if (!this->can_redo()) {
    return false;
  }

  ++this->cursor;
  if (this->cursor == this->snapshots.get_size()) {
    this->cursor = 0;
  }

  this->snapshots[this->cursor].restore(model);
  return true;
}

} // namespace history
