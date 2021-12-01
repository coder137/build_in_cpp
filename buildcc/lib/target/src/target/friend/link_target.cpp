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

#include "target/friend/link_target.h"

#include "target/target.h"

namespace {
constexpr const char *const kOutput = "output";
constexpr const char *const kCompiledSources = "compiled_sources";
constexpr const char *const kLibDeps = "lib_deps";
} // namespace

namespace buildcc::base {

// PUBLIC

void LinkTarget::CacheLinkCommand() {
  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(target_.compile_object_.GetCompiledSources());

  const std::string output_target = fmt::format("{}", output_);

  const auto &storer = target_.storer_;
  command_ = target_.command_.Construct(
      target_.config_.link_command,
      {
          {kOutput, output_target},
          {kCompiledSources, aggregated_compiled_sources},
          {kLibDeps,
           fmt::format("{} {}",
                       internal::aggregate(storer.current_external_lib_deps),
                       internal::aggregate(storer.current_lib_deps.user))},
      });
}

// PRIVATE

fs::path LinkTarget::ConstructOutputPath() const {
  fs::path path =
      target_.GetTargetBuildDir() /
      fmt::format("{}{}", target_.GetName(), target_.GetConfig().target_ext);
  path.make_preferred();
  return path;
}

void LinkTarget::PreLink() {
  auto &storer = target_.storer_;

  storer.current_lib_deps.Convert();

  storer.current_link_dependencies.Convert();
}

void LinkTarget::BuildLink() {
  PreLink();

  const auto &loader = target_.loader_;
  const auto &storer = target_.storer_;

  if (!loader.IsLoaded()) {
    target_.dirty_ = true;
  } else {
    target_.RecheckFlags(loader.GetLoadedLinkFlags(),
                         target_.GetCurrentLinkFlags());
    target_.RecheckDirs(loader.GetLoadedLibDirs(), target_.GetCurrentLibDirs());
    target_.RecheckExternalLib(loader.GetLoadedExternalLibDeps(),
                               storer.current_external_lib_deps);
    target_.RecheckPaths(loader.GetLoadedLinkDependencies(),
                         storer.current_link_dependencies.internal);
    target_.RecheckPaths(loader.GetLoadedLibDeps(),
                         storer.current_lib_deps.internal);
    if (!loader.GetLoadedTargetLinked()) {
      target_.dirty_ = true;
    }
  }

  if (target_.dirty_) {
    bool success = Command::Execute(command_);
    env::assert_throw(success, "Failed to link target");
    target_.storer_.target_linked = true;
  }
}

} // namespace buildcc::base
