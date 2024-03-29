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

#include "target/friend/link_target.h"

#include "target/target.h"

namespace {
constexpr const char *const kOutput = "output";
constexpr const char *const kCompiledSources = "compiled_sources";
constexpr const char *const kLibDeps = "lib_deps";
} // namespace

namespace buildcc::internal {

// PUBLIC

void LinkTarget::CacheLinkCommand() {
  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(target_.compile_object_.GetCompiledSources());

  const std::string output_target = fmt::format("{}", output_);
  const auto &target_user_schema = target_.user_;
  command_ = target_.command_.Construct(
      target_.GetConfig().link_command,
      {
          {kOutput, output_target},
          {kCompiledSources, aggregated_compiled_sources},
          {kLibDeps,
           fmt::format("{} {}",
                       internal::aggregate(target_user_schema.libs.GetPaths()),
                       internal::aggregate(target_user_schema.external_libs))},
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
  auto &target_user_schema = target_.user_;

  target_user_schema.libs.ComputeHashForAll();

  target_user_schema.link_dependencies.ComputeHashForAll();
}

void LinkTarget::BuildLink() {
  PreLink();

  const auto &serialization = target_.serialization_;
  const auto &target_load_schema = serialization.GetLoad();
  const auto &target_user_schema = target_.user_;

  if (!serialization.IsLoaded()) {
    target_.dirty_ = true;
  } else {
    if (target_.dirty_) {
      // Skip all the other else if checks
    } else if (!(target_load_schema.link_flags ==
                 target_user_schema.link_flags)) {
      target_.dirty_ = true;
      target_.FlagChanged();
    } else if (!(target_load_schema.lib_dirs == target_user_schema.lib_dirs)) {
      target_.dirty_ = true;
      target_.DirChanged();
    } else if (!(target_load_schema.external_libs ==
                 target_user_schema.external_libs)) {
      target_.dirty_ = true;
      target_.ExternalLibChanged();
    } else if (!(target_load_schema.link_dependencies ==
                 target_user_schema.link_dependencies) ||
               !(target_load_schema.libs == target_user_schema.libs)) {
      target_.dirty_ = true;
      target_.PathChanged();
    } else if (!target_load_schema.target_linked) {
      // TODO, Replace this with fs::exists to check if linked target is present
      // or no
      target_.dirty_ = true;
    }
  }

  if (target_.dirty_) {
    bool success = env::Command::Execute(command_);
    env::assert_fatal(success, "Failed to link target");
    target_.serialization_.UpdateTargetCompiled();
  }
}

} // namespace buildcc::internal
