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

#ifndef ENV_ASSERT_THROW_H_
#define ENV_ASSERT_THROW_H_

#include <string>

#include "logging.h"

namespace buildcc::env {

/**
 * @brief Compile time expr asserts fatally when false
 */
template <bool expr> inline void assert_throw(const char *message) {
  if constexpr (!expr) {
    env::log_critical("assert", message);
    // TODO, If needed specialize this
    throw std::exception();
  }
}

/**
 * @brief Compile time expr asserts fatally when false
 */
template <bool expr> inline void assert_throw(const std::string &message) {
  assert_throw<expr>(message.c_str());
}

/**
 * @brief Runtime expr asserts fatally when false
 */
inline void assert_throw(bool expression, const char *message) {
  if (!expression) {
    assert_throw<false>(message);
  }
}

/**
 * @brief Runtime expr asserts fatally when false
 */
inline void assert_throw(bool expression, const std::string &message) {
  assert_throw(expression, message.c_str());
}

} // namespace buildcc::env

#endif
