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

#include "target/target.h"

#include "env/assert_fatal.h"

#include "target/util.h"

namespace buildcc::base {

void Target::AddLibDir(const fs::path &relative_lib_dir) {
  env::log_trace(name_, __FUNCTION__);

  fs::path final_lib_dir = target_root_source_dir_ / relative_lib_dir;
  AddLibDirAbsolute(final_lib_dir);
}

void Target::AddLibDirAbsolute(const fs::path &absolute_lib_dir) {
  env::log_trace(name_, __FUNCTION__);

  LockedAfterBuild();
  storer_.current_lib_dirs.insert(absolute_lib_dir);
}

void Target::AddLibDep(const Target &lib_dep) {
  env::log_trace(name_, __FUNCTION__);

  LockedAfterBuild();
  storer_.current_lib_deps.user.insert(lib_dep.GetTargetPath());
}

void Target::AddLibDep(const std::string &lib_dep) {
  env::log_trace(name_, __FUNCTION__);

  LockedAfterBuild();
  storer_.current_external_lib_deps.insert(lib_dep);
}

} // namespace buildcc::base
