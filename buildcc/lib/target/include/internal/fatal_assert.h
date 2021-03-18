#ifndef BUILDCC_INCLUDE_INTERNAL_ASSERT_H_
#define BUILDCC_INCLUDE_INTERNAL_ASSERT_H_

#include <string>

#include "env.h"

namespace buildcc::internal {

template <typename T>
void assert_fatal(T first, T second, const std::string &message) {
  if (first != second) {
    buildcc::env::log_critical(message, "assert");
    throw message;
  }
}

inline void assert_fatal_true(bool first, const std::string &message) {
  assert_fatal(first, true, message);
}

} // namespace buildcc::internal

#endif
