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

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace buildcc {

class Command {
public:
  explicit Command() = default;

  /**
   * @brief Add key-value pairs that are used by fmt as identifiers
   * Example: {"key", "value"} -> fmt::format("{key}") -> "value"
   *
   * NOTE: These default arguments persist throughout the lifetime of the
   * Command object
   */
  void AddDefaultArgument(const std::string &key, const std::string &value);

  /**
   * @brief Add multiple key-value pairs that are used by fmt as identifiers
   * Example: {"key", "value"} -> fmt::format("{key}") -> "value"
   *
   * NOTE: These default arguments persist throughout the lifetime of the
   * Command object
   */
  void AddDefaultArguments(
      const std::unordered_map<std::string, std::string> &arguments);

  /**
   * @brief Construct a specialized string using input pattern and supplied
   * arguments
   *
   * NOTE: These arguments are only valid for the `Construct` function call
   */
  std::string Construct(const std::string &pattern,
                        const std::unordered_map<const char *, std::string>
                            &arguments = {}) const;

  /**
   * @brief Execute a particular command and optionally redirect stdout and
   * stderr to user supplied dynamic string lists
   */
  static bool Execute(const std::string &command,
                      const std::optional<fs::path> &working_directory = {},
                      std::vector<std::string> *stdout_data = nullptr,
                      std::vector<std::string> *stderr_data = nullptr);

  /**
   * @brief Get the Default Value By Key object
   * NOTE: Only works when key/value pairs are added to DefaultArgument(s)
   * Assert Fatal if default value is not found
   */
  const std::string &GetDefaultValueByKey(const std::string &key) const;

private:
  std::unordered_map<std::string, std::string> default_values_;
};

} // namespace buildcc

#endif
