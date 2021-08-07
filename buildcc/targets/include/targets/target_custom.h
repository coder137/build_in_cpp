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

#ifndef TARGETS_TARGET_CUSTOM_H_
#define TARGETS_TARGET_CUSTOM_H_

#include "target_constants.h"

namespace buildcc {

class Target_custom : public base::Target {
public:
  Target_custom(const std::string &name, base::TargetType type,
                const base::Toolchain &toolchain,
                const std::filesystem::path &target_path_relative_to_root,
                std::string_view compile_command, std::string_view link_command)
      : Target(name, type, toolchain, target_path_relative_to_root) {
    compile_command_ = compile_command;
    link_command_ = link_command;
  }
};

} // namespace buildcc

#endif
