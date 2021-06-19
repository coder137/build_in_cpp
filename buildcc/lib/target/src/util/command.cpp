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

#include "internal/command.h"
#include "internal/util.h"

#include "logging.h"

#include "process.hpp"

namespace tpl = TinyProcessLib;

namespace buildcc::internal {

void Command::AddDefaultArguments(
    std::initializer_list<fmt::detail::named_arg<char, std::string>>
        arguments) {
  for (const auto &a : arguments) {
    default_values_.push_back(a);
  }
}

std::string Command::Construct(
    std::string_view format,
    std::initializer_list<fmt::detail::named_arg<char, std::string>> arguments)
    const {
  // Construct your arguments
  fmt::dynamic_format_arg_store<fmt::format_context> store;
  for (const auto &v : default_values_) {
    store.push_back(v);
  }
  for (const auto &a : arguments) {
    store.push_back(a);
  }

  // Construct your command
  std::string constructed_string{""};
  try {
    constructed_string = fmt::vformat(format, store);
  } catch (const std::exception &e) {
    env::assert_fatal(false, e.what());
  }
  return constructed_string;
}

bool Command::Execute(const std::string &command) const {
  // Run the process
  buildcc::env::log_debug("system", command);
  tpl::Process process(command);
  return process.get_exit_status() == 0;
}

} // namespace buildcc::internal
