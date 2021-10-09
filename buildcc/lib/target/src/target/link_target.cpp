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

// Construct APIs
std::string Target::ConstructLinkCommand() const {
  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(GetCompiledSources());

  const std::string output_target =
      internal::Path::CreateNewPath(GetTargetPath()).GetPathAsString();

  return command_.Construct(
      link_command_,
      {
          {"output", output_target},
          {"compiled_sources", aggregated_compiled_sources},
          {"lib_deps",
           fmt::format("{} {}", internal::aggregate(current_external_lib_deps_),
                       internal::aggregate(current_lib_deps_.user))},
      });
}

// Link APIs
void Target::PreLink() {
  for (const auto &user_ld : current_lib_deps_.user) {
    current_lib_deps_.internal.emplace(
        internal::Path::CreateExistingPath(user_ld));
  }

  for (const auto &user_ld : current_link_dependencies_.user) {
    current_link_dependencies_.internal.emplace(
        internal::Path::CreateExistingPath(user_ld));
  }
}

void Target::BuildLink() {
  PreLink();

  RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
  RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
  RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                     current_external_lib_deps_);
  RecheckPaths(loader_.GetLoadedLinkDependencies(),
               current_link_dependencies_.internal);
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_.internal);

  if (dirty_) {
    bool success = Command::Execute(current_target_file_.command);
    env::assert_fatal(success, "Failed to link target");
    Store();
    build_ = true;
  }
}

} // namespace buildcc::base
