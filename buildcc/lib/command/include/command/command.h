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

#ifndef COMMAND_COMMAND_H_
#define COMMAND_COMMAND_H_

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace buildcc {

class Command {
public:
  explicit Command() = default;

  void AddDefaultArgument(const std::string &key, const std::string &value);
  void AddDefaultArguments(
      const std::unordered_map<std::string, std::string> &arguments);

  std::string Construct(std::string_view format,
                        const std::unordered_map<const char *, std::string>
                            &arguments = {}) const;

  static bool Execute(const std::string &command,
                      std::vector<std::string> *stdout_data = nullptr,
                      std::vector<std::string> *stderr_data = nullptr);

  /**
   * @brief Get the Default Value By Key object
   * NOTE: Only works when key/value pairs are added to DefaultArgument(s)
   *
   * @return const std::string&
   */
  const std::string &GetDefaultValueByKey(const std::string &key) const;

private:
  std::unordered_map<std::string, std::string> default_values_;
};

} // namespace buildcc

#endif
