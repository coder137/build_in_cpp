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

#include <algorithm>

#include "fmt/format.h"
#include "logging.h"

namespace buildcc::internal {

void Command::AddDefaultArguments(
    const std::unordered_map<const char *, std::string> &arguments) {
  default_values_.insert(arguments.begin(), arguments.end());
}

std::string Command::Construct(
    std::string_view format,
    const std::unordered_map<const char *, std::string> &arguments) const {
  // Construct your arguments
  fmt::dynamic_format_arg_store<fmt::format_context> store;
  std::string constructed_string;
  try {

    std::for_each(default_values_.cbegin(), default_values_.cend(),
                  [&store](const std::pair<const char *, std::string> &p) {
                    store.push_back(fmt::arg(p.first, p.second));
                  });

    std::for_each(arguments.cbegin(), arguments.cend(),
                  [&store](const std::pair<const char *, std::string> &p) {
                    store.push_back(fmt::arg(p.first, p.second));
                  });

    // Construct your command
    constructed_string = fmt::vformat(format, store);
  } catch (const std::exception &e) {
    env::assert_fatal(false, e.what());
  }
  return constructed_string;
}

bool Command::ConstructAndExecute(
    std::string_view format,
    const std::unordered_map<const char *, std::string> &arguments) const {
  const std::string constructed_command = Construct(format, arguments);
  return Execute(constructed_command);
}

} // namespace buildcc::internal
