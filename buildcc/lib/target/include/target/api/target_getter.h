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

#ifndef TARGET_API_TARGET_GETTER_H_
#define TARGET_API_TARGET_GETTER_H_

#include <filesystem>
#include <string>

#include "target/common/target_type.h"

#include "toolchain/toolchain.h"

#include "taskflow/taskflow.hpp"

namespace fs = std::filesystem;

namespace buildcc::base {

template <typename T> class TargetGetter {
public:
  const std::string &GetName() const;
  const Toolchain &GetToolchain() const;
  TargetType GetType() const;

  const fs::path &GetTargetPath() const;
  const fs::path &GetBinaryPath() const;

  const fs::path &GetPchHeaderPath() const;
  const fs::path &GetPchCompilePath() const;

  const std::string &GetCompileCommand(const fs::path &source) const;
  const std::string &GetLinkCommand() const;

  tf::Taskflow &GetTaskflow();
};

}; // namespace buildcc::base

#endif
