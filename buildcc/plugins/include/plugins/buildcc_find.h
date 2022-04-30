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

#ifndef PLUGINS_BUILDCC_FIND_H_
#define PLUGINS_BUILDCC_FIND_H_

#include <filesystem>
#include <initializer_list>
#include <string>
#include <vector>

#include "env/host_os.h"

#include "fmt/format.h"

namespace fs = std::filesystem;

namespace buildcc::plugin {

/**
 * @brief By default searched the `PATH` environment variable for required
 * package
 *
 */
// TODO, Add support for regex matching
class BuildccFind {
public:
  // TODO, Add more
  enum class Type {
    HostExecutable,
    BuildccLibrary,
    BuildccPlugin,
  };

public:
  BuildccFind(const std::string &regex, Type type,
              std::initializer_list<std::string> host_env_vars = {})
      : regex_(regex), type_(type), host_env_vars_(host_env_vars) {
    if constexpr (env::is_win()) {
      regex_ = fmt::format("{}.exe", regex);
    }
  }
  BuildccFind(const BuildccFind &) = delete;

  void AddHostEnvVar(const std::string &host_env_var) {
    host_env_vars_.push_back(host_env_var);
  }
  void AddHostEnvVars(std::initializer_list<std::string> host_env_vars) {
    for (const auto &ev : host_env_vars) {
      AddHostEnvVar(ev);
    }
  }

  /**
   * @brief Starts the search for package of type and name
   *
   */
  bool Search();

  //   Getters
  const std::vector<fs::path> &GetPathMatches() { return target_matches_; }

private:
  bool SearchHostExecutable();

private:
  std::string regex_;
  Type type_;

  std::vector<std::string> host_env_vars_;

  std::vector<fs::path> target_matches_;
};

} // namespace buildcc::plugin

#endif
