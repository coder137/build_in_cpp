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

namespace {
constexpr const char *const kOutput = "output";
constexpr const char *const kCompiledSources = "compiled_sources";
constexpr const char *const kLibDeps = "lib_deps";
} // namespace

namespace buildcc::base {

// Construct APIs
std::string Target::ConstructLinkCommand() const {
  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(compile_object_.GetCompiledSources());

  const std::string output_target =
      internal::Path::CreateNewPath(GetTargetPath()).GetPathAsString();

  return command_.Construct(
      config_.link_command,
      {
          {kOutput, output_target},
          {kCompiledSources, aggregated_compiled_sources},
          {kLibDeps,
           fmt::format("{} {}",
                       internal::aggregate(storer_.current_external_lib_deps),
                       internal::aggregate(storer_.current_lib_deps.user))},
      });
}

// Link APIs
void Target::PreTargetLink() {
  storer_.current_lib_deps.Convert();

  storer_.current_link_dependencies.Convert();
}

void Target::BuildTargetLink() {
  PreTargetLink();

  RecheckFlags(loader_.GetLoadedLinkFlags(), GetCurrentLinkFlags());
  RecheckDirs(loader_.GetLoadedLibDirs(), GetCurrentLibDirs());
  RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                     storer_.current_external_lib_deps);
  RecheckPaths(loader_.GetLoadedLinkDependencies(),
               storer_.current_link_dependencies.internal);
  RecheckPaths(loader_.GetLoadedLibDeps(), storer_.current_lib_deps.internal);

  if (dirty_) {
    bool success = Command::Execute(GetLinkCommand());
    env::assert_fatal(success, "Failed to link target");
    Store();
    state_.build = true;
  }
}

} // namespace buildcc::base
