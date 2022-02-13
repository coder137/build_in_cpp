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

#ifndef TARGET_TARGET_H_
#define TARGET_TARGET_H_

#include <filesystem>
#include <functional>
#include <initializer_list>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Interface
#include "target/interface/builder_interface.h"

// API
#include "target/api/target_getter.h"
#include "target/target_info.h"

// Common
#include "schema/target_type.h"

// Friend
#include "target/friend/compile_object.h"
#include "target/friend/compile_pch.h"
#include "target/friend/link_target.h"

// Internal
#include "schema/path.h"
#include "schema/target_serialization.h"

// Env
#include "env/env.h"
#include "env/task_state.h"

// Components
#include "env/command.h"
#include "toolchain/toolchain.h"

// Third Party
#include "taskflow/taskflow.hpp"

namespace buildcc {

// NOTE, BaseTarget is meant to be a blank slate which can be customized by
// the specialized target-toolchain classes
class Target : public internal::BuilderInterface,
               public TargetInfo,
               public internal::TargetGetter<Target> {

public:
  explicit Target(const std::string &name, TargetType type,
                  const Toolchain &toolchain, const TargetEnv &env,
                  const TargetConfig &config = TargetConfig())
      : TargetInfo(
            TargetEnv(env.GetTargetRootDir(),
                      env.GetTargetBuildDir() / toolchain.GetName() / name),
            config),
        name_(name), type_(type), toolchain_(toolchain),
        // loader_(name, env_.GetTargetBuildDir()),
        serialization_(env_.GetTargetBuildDir() / fmt::format("{}.bin", name)),
        compile_pch_(*this), compile_object_(*this), link_target_(*this) {
    Initialize();
  }
  virtual ~Target() {}
  Target(const Target &target) = delete;

  // Builders
  void Build() override;

  // Getters
  env::TaskState GetTaskState() const noexcept { return task_state_; }

private:
  friend class internal::CompilePch;
  friend class internal::CompileObject;
  friend class internal::LinkTarget;

  friend class internal::TargetGetter<Target>;

private:
  void Initialize();

  //
  std::optional<std::string> SelectCompileFlags(TargetFileExt ext) const;
  std::optional<std::string> SelectCompiler(TargetFileExt ext) const;

  // Recompilation checks
  void RecheckPaths(const internal::path_unordered_set &previous_path,
                    const internal::path_unordered_set &current_path);
  void RecheckDirs(const fs_unordered_set &previous_dirs,
                   const fs_unordered_set &current_dirs);
  void RecheckFlags(const std::unordered_set<std::string> &previous_flags,
                    const std::unordered_set<std::string> &current_flags);
  void
  RecheckExternalLib(const std::vector<std::string> &previous_external_libs,
                     const std::vector<std::string> &current_external_libs);

  // Tasks
  void SetTaskStateFailure();
  int GetTaskStateAsInt() const noexcept {
    return static_cast<int>(task_state_);
  }

  void StartTask();
  void EndTask();
  tf::Task CheckStateTask();
  void TaskDeps();

  // Callbacks for unit tests
  void SourceRemoved();
  void SourceAdded();
  void SourceUpdated();
  void PathRemoved();
  void PathAdded();
  void PathUpdated();

  void DirChanged();
  void FlagChanged();
  void ExternalLibChanged();

private:
  std::string name_;
  TargetType type_;
  const Toolchain &toolchain_;
  internal::TargetSerialization serialization_;

  // Friend classes
  internal::CompilePch compile_pch_;
  internal::CompileObject compile_object_;
  internal::LinkTarget link_target_;

  // Task states
  tf::Task target_start_task_;
  tf::Task target_end_task_;

  std::mutex task_state_mutex_;
  env::TaskState task_state_{env::TaskState::SUCCESS};

  //
  env::Command command_;
  tf::Taskflow tf_;
};

typedef Target BaseTarget;

} // namespace buildcc

#endif
