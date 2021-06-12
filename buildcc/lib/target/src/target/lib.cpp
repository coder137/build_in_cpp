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

#include "target.h"

#include "assert_fatal.h"

#include "internal/util.h"

namespace buildcc::base {

void Target::AddLibDir(const fs::path &relative_lib_dir) {
  env::log_trace(name_, __FUNCTION__);

  fs::path final_lib_dir = target_root_source_dir_ / relative_lib_dir;
  AddLibDirAbsolute(final_lib_dir);
}

void Target::AddLibDirAbsolute(const fs::path &absolute_lib_dir) {
  env::log_trace(name_, __FUNCTION__);

  current_lib_dirs_.insert(
      internal::Path::CreateNewPath(absolute_lib_dir).GetPathAsString());
}

void Target::AddLibDep(const Target &lib_dep) {
  env::log_trace(name_, __FUNCTION__);

  const fs::path lib_dep_path = lib_dep.GetTargetPath();
  internal::add_path(lib_dep_path, current_lib_deps_);
}

void Target::AddLibDep(const std::string &lib_dep) {
  env::log_trace(name_, __FUNCTION__);
  current_external_lib_deps_.insert(lib_dep);
}

} // namespace buildcc::base
