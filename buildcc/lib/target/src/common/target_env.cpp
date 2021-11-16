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

#include "target/common/target_env.h"

namespace buildcc::base {

// TODO, Remove this eventually
TargetEnv::TargetEnv(const char *target_relative_to_env_root)
    : target_root_dir_(env::get_project_root_dir() /
                       target_relative_to_env_root),
      target_build_dir_(env::get_project_build_dir()), relative_(true) {}

TargetEnv::TargetEnv(const fs::path &target_relative_to_env_root)
    : target_root_dir_(env::get_project_root_dir() /
                       target_relative_to_env_root),
      target_build_dir_(env::get_project_build_dir()), relative_(true) {}
TargetEnv::TargetEnv(const fs::path &absolute_target_root,
                     const fs::path &absolute_target_build)
    : target_root_dir_(absolute_target_root),
      target_build_dir_(absolute_target_build), relative_(false) {}

} // namespace buildcc::base
