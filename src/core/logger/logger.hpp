/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-27
 *===============================*/

#ifndef PXL_LOGGER_HPP
#define PXL_LOGGER_HPP

#include "types.hpp"

namespace logger {

enum Level {
  SILENT_LVL = 0,
  FATAL_LVL = 1,
  ERROR_LVL = 2,
  WARN_LVL = 3,
  INFO_LVL = 4,
  DEBUG_LVL = 5
};

void set_level(Level lvl) noexcept;
void fatal(const c8* msg, ...) noexcept;
void error(const c8* msg, ...) noexcept;
void warn(const c8* msg, ...) noexcept;
void info(const c8* msg, ...) noexcept;
void debug(const c8* msg, ...) noexcept;

} // namespace logger

#endif
