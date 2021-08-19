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

#ifndef TARGET_COMMAND_H_
#define TARGET_COMMAND_H_

#include <string>
#include <string_view>
#include <unordered_map>

namespace buildcc::internal {

class Command {
public:
  explicit Command() = default;

  void AddDefaultArguments(
      const std::unordered_map<const char *, std::string> &arguments);

  std::string Construct(std::string_view format,
                        const std::unordered_map<const char *, std::string>
                            &arguments = {}) const;
  bool ConstructAndExecute(std::string_view format,
                           const std::unordered_map<const char *, std::string>
                               &arguments = {}) const;

  static bool Execute(const std::string &command);

private:
  std::unordered_map<const char *, std::string> default_values_;
};

} // namespace buildcc::internal

#endif
