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
      : TargetInfo(toolchain, TargetEnv(env.GetTargetRootDir(),
                                        env.GetTargetBuildDir() /
                                            toolchain.GetName() / name)),
        name_(name), type_(type), config_(config),
        serialization_(env_.GetTargetBuildDir() / fmt::format("{}.bin", name)),
        compile_pch_(*this), compile_object_(*this), link_target_(*this) {
    Initialize();
  }
  virtual ~Target() = default;
  Target(const Target &target) = delete;

  // Builders
  void Build() override;

private:
  friend class internal::CompilePch;
  friend class internal::CompileObject;
  friend class internal::LinkTarget;

  friend class internal::TargetGetter<Target>;

private:
  void Initialize();

  //
  env::optional<std::string> SelectCompileFlags(FileExt ext) const;
  env::optional<std::string> SelectCompiler(FileExt ext) const;

  // Tasks
  void EndTask();
  void TaskDeps();

  // Callbacks for unit tests
  void SourceRemoved();
  void SourceAdded();
  void SourceUpdated();
  void PathRemoved();
  void PathAdded();
  void PathUpdated();

  void PathChanged();
  void DirChanged();
  void FlagChanged();
  void ExternalLibChanged();

private:
  std::string name_;
  TargetType type_;
  TargetConfig config_;
  internal::TargetSerialization serialization_;
  internal::CompilePch compile_pch_;
  internal::CompileObject compile_object_;
  internal::LinkTarget link_target_;

  //
  TargetState state_;
  env::Command command_;

  // Task states
  tf::Task target_start_task_;
  tf::Task target_end_task_;
};

typedef Target BaseTarget;

} // namespace buildcc

#endif
