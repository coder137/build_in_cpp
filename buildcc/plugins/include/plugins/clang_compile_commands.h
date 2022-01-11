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

#ifndef PLUGINS_CLANG_COMPILE_COMMANDS_H_
#define PLUGINS_CLANG_COMPILE_COMMANDS_H_

#include "target/target.h"

namespace buildcc::plugin {

class ClangCompileCommands {
public:
  explicit ClangCompileCommands(std::vector<const base::Target *> &&targets)
      : targets_(targets) {}
  ClangCompileCommands(const ClangCompileCommands &compile_commands) = delete;

  /**
   * @brief Add non-null targets
   */
  void AddTarget(const base::Target *target);

  /**
   * @brief Generate clang compile commands file in `env::get_project_build_dir`
   * folder
   */
  void Generate();

private:
  std::vector<const base::Target *> targets_;
};

} // namespace buildcc::plugin

#endif
