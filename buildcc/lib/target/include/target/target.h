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

#ifndef TARGET_TARGET_H_
#define TARGET_TARGET_H_

#include <filesystem>
#include <functional>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Interface
#include "target/builder_interface.h"

// API
#include "target/target_info.h"

// Common
#include "target/common/target_type.h"

// Friend
#include "target/friend/compile_object.h"
#include "target/friend/compile_pch.h"
#include "target/friend/link_target.h"

// Internal
#include "target/path.h"
#include "target/target_loader.h"
#include "target/target_storer.h"

// Components
#include "command/command.h"
#include "env/env.h"
#include "toolchain/toolchain.h"

// Third Party
#include "taskflow/taskflow.hpp"

namespace buildcc::base {

// TODO, Discuss if this should be marked final
// For specialized target-toolchain classes we could force composition instead
// of the inheritance pattern
// NOTE, base::Target is meant to be a blank slate which can be customized by
// the specialized target-toolchain classes
class Target : public BuilderInterface, public TargetInfo {

public:
  explicit Target(const std::string &name, TargetType type,
                  const Toolchain &toolchain, const TargetEnv &env,
                  const TargetConfig &config = TargetConfig())
      : TargetInfo(
            TargetEnv(env.GetTargetRootDir(),
                      env.GetTargetBuildDir() / toolchain.GetName() / name),
            config),
        name_(name), type_(type), toolchain_(toolchain),
        loader_(name, env_.GetTargetBuildDir()), compile_pch_(*this),
        compile_object_(*this), link_target_(*this) {
    Initialize();
  }
  virtual ~Target() {}
  Target(const Target &target) = delete;

  // Builders
  void Build() override;

  // Getters (GENERIC)

  // NOTE, We are constructing the path
  fs::path GetBinaryPath() const { return loader_.GetBinaryPath(); }
  const fs::path &GetTargetPath() const { return link_target_.GetOutput(); }
  const fs::path &GetPchHeaderPath() const {
    return compile_pch_.GetHeaderPath();
  }
  const fs::path &GetPchCompilePath() const {
    return compile_pch_.GetCompilePath();
  }

  // Const references

  // TODO, Shift getters to source file as well
  const std::string &GetName() const { return name_; }
  const Toolchain &GetToolchain() const { return toolchain_; }
  TargetType GetType() const { return type_; }

  //

  // Getters (state_.ExpectsLock)

  const std::string &GetCompileCommand(const fs::path &source) const {
    state_.ExpectsLock();
    return compile_object_.GetObjectData(source).command;
  }
  const std::string &GetLinkCommand() const {
    state_.ExpectsLock();
    return link_target_.GetCommand();
  }

  tf::Taskflow &GetTaskflow() {
    state_.ExpectsLock();
    return tf_;
  }

  // TODO, Add more getters

private:
  friend class CompilePch;
  friend class CompileObject;
  friend class LinkTarget;

  friend class CopyApi<Target>;
  friend class SourceApi<Target>;
  friend class IncludeApi<Target>;
  friend class LibApi<Target>;
  friend class PchApi<Target>;
  friend class FlagApi<Target>;
  friend class DepsApi<Target>;

private:
  void Initialize();

  //
  std::optional<std::string> SelectCompileFlags(TargetFileExt ext) const;
  std::optional<std::string> SelectCompiler(TargetFileExt ext) const;

  // Recompilation checks
  void RecheckPaths(const internal::path_unordered_set &previous_path,
                    const internal::path_unordered_set &current_path);
  void RecheckDirs(const internal::fs_unordered_set &previous_dirs,
                   const internal::fs_unordered_set &current_dirs);
  void RecheckFlags(const std::unordered_set<std::string> &previous_flags,
                    const std::unordered_set<std::string> &current_flags);
  void RecheckExternalLib(
      const std::unordered_set<std::string> &previous_external_libs,
      const std::unordered_set<std::string> &current_external_libs);

  // Fbs
  bool Store() override;

  // Tasks
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
  internal::TargetLoader loader_;

  // Friend classes
  CompilePch compile_pch_;
  CompileObject compile_object_;
  LinkTarget link_target_;

  //
  Command command_;
  tf::Taskflow tf_;
};

} // namespace buildcc::base

// TODO, Make all of these external and remove this namespace
namespace buildcc {

typedef base::TargetType TargetType;
typedef base::TargetState TargetState;
typedef base::TargetConfig TargetConfig;
typedef base::TargetEnv TargetEnv;

typedef base::Target BaseTarget;

} // namespace buildcc

#endif
