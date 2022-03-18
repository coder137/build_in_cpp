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

#ifndef TARGET_API_TARGET_GETTER_H_
#define TARGET_API_TARGET_GETTER_H_

#include <filesystem>
#include <string>

#include "schema/target_type.h"

#include "toolchain/toolchain.h"

#include "target/common/target_config.h"
#include "target/common/target_state.h"

#include "taskflow/taskflow.hpp"

namespace fs = std::filesystem;

namespace buildcc::internal {

template <typename T> class TargetGetter {
public:
  // Target State
  const TargetState &GetState() const;
  bool IsBuilt() const;
  bool IsLocked() const;

  // Target Config
  const TargetConfig &GetConfig() const;

  const std::string &GetName() const;
  const Toolchain &GetToolchain() const;
  TargetType GetType() const;

  /**
   * @brief Location of generated Target
   */
  const fs::path &GetTargetPath() const;

  /**
   * @brief Location of serialized Target data
   */
  const fs::path &GetBinaryPath() const;

  /**
   * @brief BuildCC constructed PCH header file
   * Example:
   * - {file}.h
   */
  const fs::path &GetPchHeaderPath() const;

  /**
   * @brief PCH compiled file
   * Example:
   * - {file}.gch for GCC
   * - {file}.pch for MSVC
   */
  const fs::path &GetPchCompilePath() const;

  // TODO, Add GetPchCommand if required
  const std::string &GetCompileCommand(const fs::path &source) const;
  const std::string &GetLinkCommand() const;

  tf::Taskflow &GetTaskflow();
};

}; // namespace buildcc::internal

#endif
