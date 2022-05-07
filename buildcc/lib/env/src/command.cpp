/*
 * Copyright 2021-2022 Niket Naidu. All rights reserved.
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

#include "env/command.h"

#include <algorithm>

#include "fmt/args.h"
#include "fmt/format.h"

#include "env/assert_fatal.h"
#include "env/logging.h"

namespace buildcc::env {

void Command::AddDefaultArgument(const std::string &key,
                                 const std::string &value) {
  default_values_.emplace(key, value);
}

void Command::AddDefaultArguments(
    const std::unordered_map<std::string, std::string> &arguments) {
  default_values_.insert(arguments.begin(), arguments.end());
}

const std::string &Command::GetDefaultValueByKey(const std::string &key) const {
  const auto iter = default_values_.find(key);
  env::assert_fatal(!(iter == default_values_.end()),
                    fmt::format("Could not find value for '{}'", key));
  return default_values_.at(key);
}

std::string Command::Construct(
    const std::string &pattern,
    const std::unordered_map<const char *, std::string> &arguments) const {
  // Construct your arguments
  fmt::dynamic_format_arg_store<fmt::format_context> store;
  std::for_each(default_values_.cbegin(), default_values_.cend(),
                [&store](const std::pair<std::string, std::string> &p) {
                  store.push_back(fmt::arg(p.first.c_str(), p.second));
                });

  std::for_each(arguments.cbegin(), arguments.cend(),
                [&store](const std::pair<const char *, std::string> &p) {
                  env::assert_fatal(p.first != NULL,
                                    "Argument must not be NULL");
                  store.push_back(fmt::arg(p.first, p.second));
                });

  // Construct your command
  std::string ret;
  try {
    ret = fmt::vformat(pattern, store);
  } catch (const std::exception &e) {
    env::assert_fatal<false>(
        fmt::format("Construct command failed: {}", e.what()));
  }
  return ret;
}

} // namespace buildcc::env
