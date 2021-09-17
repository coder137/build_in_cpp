/*
 * Copyright 2021 Niket Naidu. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ENV_ASSERT_FATAL_H_
#define ENV_ASSERT_FATAL_H_

#include <string>

#include "logging.h"

namespace buildcc::env {

/**
 * @brief During Release -> std::terminate
 * During Unit Test -> throw std::exception
 */
[[noreturn]] void assert_handle_fatal();

template <bool expr> inline void assert_fatal(const char *message) {
  if constexpr (!expr) {
    env::log_critical("assert", message);
    assert_handle_fatal();
  }
}

template <bool expr> inline void assert_fatal(const std::string &message) {
  assert_fatal<expr>(message.c_str());
}

inline void assert_fatal(bool expression, const char *message) {
  if (!expression) {
    assert_fatal<false>(message);
  }
}

inline void assert_fatal(bool expression, const std::string &message) {
  assert_fatal(expression, message.c_str());
}

} // namespace buildcc::env

#define ASSERT_FATAL(expr, message)                                            \
  ((expr) ? static_cast<void>(0) : buildcc::env::assert_fatal<false>(message))

#endif
