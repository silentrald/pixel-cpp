/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-29
 *===============================*/

#include "./caretaker.hpp"
#include "core/logger/logger.hpp"
#include "types.hpp"
#include <cassert>

namespace history {

Caretaker::Caretaker(i32 capacity) noexcept {
  this->snapshots.resize(capacity);
}

void Caretaker::init(Snapshot&& snapshot) noexcept {
  this->clear();
  this->snapshots[0] = std::move(snapshot);
  this->cursor = this->start_cursor = this->end_cursor = 0;
}

void Caretaker::push_snapshot(Snapshot&& snapshot) noexcept {
  this->remove_future_snapshots();

  // Move the cursor and move the new snapshot
  ++this->cursor;
  if (this->cursor == this->snapshots.size()) {
    this->cursor = 0;
  }
  this->snapshots[this->cursor] = std::move(snapshot);

  this->update_cursors();
}

void Caretaker::clear() noexcept {
  for (auto& snapshot : this->snapshots) {
    snapshot.reset();
  }
}

void Caretaker::remove_future_snapshots() noexcept {
  if (this->cursor == this->end_cursor) {
    return;
  }

  if (this->cursor < this->end_cursor) {
    // +2 because +1 will contain the new snapshot
    logger::debug("Clearing %d -> %d", this->cursor + 2, this->end_cursor);
    for (i32 i = this->cursor + 2; i <= this->end_cursor; ++i) {
      this->snapshots[i].reset();
    }
    return;
  }

  logger::debug(
      "Clearing 0 -> %d & %d -> %d", this->end_cursor, this->cursor + 2,
      this->snapshots.size()
  );
  for (i32 i = this->cursor + 2; i < this->snapshots.size(); ++i) {
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

const Snapshot& Caretaker::undo() noexcept {
  assert(this->can_undo());
  if (this->cursor == 0) {
    this->cursor = this->snapshots.size() - 1;
  } else {
    --this->cursor;
  }
  logger::debug("Cursor at %d", this->cursor);
  return this->snapshots[this->cursor];
}

bool Caretaker::can_redo() const noexcept {
  return this->cursor != this->end_cursor;
}

const Snapshot& Caretaker::redo() noexcept {
  assert(this->can_redo());
  ++this->cursor;
  if (this->cursor == this->snapshots.size()) {
    this->cursor = 0;
  }
  logger::debug("Cursor at %d", this->cursor);
  return this->snapshots[this->cursor];
}

} // namespace history
