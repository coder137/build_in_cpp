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

#ifndef ENV_INCLUDE_ASSERT_FATAL_H_
#define ENV_INCLUDE_ASSERT_FATAL_H_

#include <string>

#include "logging.h"

namespace buildcc::env {

class assert_exception : public std::exception {
public:
  assert_exception(const char *const message) : message_(message) {}

private:
  virtual const char *what() const throw() { return message_; }

private:
  const char *const message_;
};

inline void assert_fatal(bool expression, const std::string &message) {
  if (!expression) {
    buildcc::env::log_critical("assert", message);
    throw assert_exception(message.c_str());
  }
}

} // namespace buildcc::env

#endif
