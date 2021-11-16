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

namespace buildcc::base {

void Target::AddCompileDependencyAbsolute(const fs::path &absolute_path) {
  state_.ExpectsUnlock();
  storer_.current_compile_dependencies.user.insert(absolute_path);
}
void Target::AddLinkDependencyAbsolute(const fs::path &absolute_path) {
  state_.ExpectsUnlock();
  storer_.current_link_dependencies.user.insert(absolute_path);
}

void Target::AddCompileDependency(const fs::path &relative_path) {
  fs::path absolute_path = GetTargetRootDir() / relative_path;
  AddCompileDependencyAbsolute(absolute_path);
}
void Target::AddLinkDependency(const fs::path &relative_path) {
  fs::path absolute_path = GetTargetRootDir() / relative_path;
  AddLinkDependencyAbsolute(absolute_path);
}

} // namespace buildcc::base
