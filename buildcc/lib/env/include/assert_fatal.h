#ifndef ENV_INCLUDE_ASSERT_FATAL_H_
#define ENV_INCLUDE_ASSERT_FATAL_H_

#include <error.h>
#include <string>

#include "env.h"

namespace buildcc::env {

inline void assert_fatal(bool expression, const std::string &message) {
  if (!expression) {
    buildcc::env::log_critical(message, "assert");
    throw message;
  }
}

} // namespace buildcc::env

#endif
