#ifndef BUILDCC_INCLUDE_INTERNAL_ASSERT_H_
#define BUILDCC_INCLUDE_INTERNAL_ASSERT_H_

#include <string>

#include "spdlog/spdlog.h"

namespace buildcc {

template <typename T>
void assert_fatal(T first, T second, const std::string &message) {
  if (first != second) {
    spdlog::error(message);
    throw message;
  }
}

template <typename T>
void assert_fatal_true(T first, const std::string &message) {
  assert_fatal(first, true, message);
}

} // namespace buildcc

#endif
