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

// Friend
#include "target/friend/compile_object.h"
#include "target/friend/compile_pch.h"
#include "target/friend/file_extension.h"
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
class Target : public BuilderInterface {

public:
  enum class Type {
    Executable,
    StaticLibrary,
    DynamicLibrary,
  };

  enum class CopyOption {
    SourceFiles,
    HeaderFiles,
    PchFiles,
    LibDeps,
    IncludeDirs,
    LibDirs,
    ExternalLibDeps,
    PreprocessorFlags,
    CommonCompileFlags,
    PchCompileFlags,
    PchObjectFlags,
    AsmCompileFlags,
    CCompileFlags,
    CppCompileFlags,
    LinkFlags,
    CompileDependencies,
    LinkDependencies,
  };

  // Defaults set for the GCC compiler
  struct Config {
    Config() {}

    std::string target_ext{""};
    std::string obj_ext{".o"};
    std::string pch_header_ext{".h"};
    std::string pch_compile_ext{".gch"};

    std::string prefix_include_dir{"-I"};
    std::string prefix_lib_dir{"-L"};

    std::string pch_command{"{compiler} {preprocessor_flags} {include_dirs} "
                            "{common_compile_flags} {pch_compile_flags} "
                            "{compile_flags} -o {output} -c {input}"};
    std::string compile_command{
        "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} "
        "{pch_object_flags} {compile_flags} -o {output} -c {input}"};
    std::string link_command{
        "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
        "{lib_dirs} {lib_deps}"};

    std::unordered_set<std::string> valid_c_ext{".c"};
    std::unordered_set<std::string> valid_cpp_ext{".cpp", ".cxx", ".cc"};
    std::unordered_set<std::string> valid_asm_ext{".s", ".S", ".asm"};
    std::unordered_set<std::string> valid_header_ext{".h", ".hpp"};
  };

  struct State {
    bool contains_pch{false};
    bool contains_asm{false};
    bool contains_c{false};
    bool contains_cpp{false};
    bool build{false};
    bool lock{false};
  };

  struct Env {
    // * NOTE, This has only been added for implicit conversion
    // TODO, Make the constructors below explicit
    // TODO, Remove this constructor
    Env(const char *target_relative_to_env_root)
        : target_root_dir(env::get_project_root_dir() /
                          target_relative_to_env_root),
          target_build_dir(env::get_project_build_dir()), relative(true) {}

    Env(const fs::path &target_relative_to_env_root)
        : target_root_dir(env::get_project_root_dir() /
                          target_relative_to_env_root),
          target_build_dir(env::get_project_build_dir()), relative(true) {}
    Env(const fs::path &absolute_target_root,
        const fs::path &absolute_target_build)
        : target_root_dir(absolute_target_root),
          target_build_dir(absolute_target_build), relative(false) {}

    fs::path target_root_dir;
    fs::path target_build_dir;
    bool relative{false};
  };

public:
  explicit Target(const std::string &name, Type type,
                  const Toolchain &toolchain, const Env &env,
                  const Config &config = {})
      : name_(name), type_(type), toolchain_(toolchain), config_(config),
        env_(env.target_root_dir,
             env.target_build_dir / toolchain.GetName() / name),
        loader_(name, env_.target_build_dir), ext_(*this), compile_pch_(*this),
        compile_object_(*this), link_target_(*this) {
    Initialize();
  }
  virtual ~Target() {}

  Target(const Target &target) = delete;

  // Features
  void Copy(const Target &target, std::initializer_list<CopyOption> options);

  // Builders
  void Build() override;

  // Setters

  // * Sources
  void AddSource(const fs::path &relative_filename,
                 const fs::path &relative_to_target_path = "");
  void GlobSources(const fs::path &relative_to_target_path);

  // Use these APIs for out of project root builds
  // Manually specify input and output
  void AddSourceAbsolute(const fs::path &absolute_input_filepath);
  void GlobSourcesAbsolute(const fs::path &absolute_input_path);

  // * Headers
  void AddHeader(const fs::path &relative_filename,
                 const fs::path &relative_to_target_path = "");
  void AddHeaderAbsolute(const fs::path &absolute_filepath);

  void GlobHeaders(const fs::path &relative_to_target_path);
  void GlobHeadersAbsolute(const fs::path &absolute_path);

  // PCH
  void AddPch(const fs::path &relative_filename,
              const fs::path &relative_to_target_path = "");
  void AddPchAbsolute(const fs::path &absolute_filepath);

  // * Include and Lib directory
  void AddIncludeDir(const fs::path &relative_include_dir,
                     bool glob_headers = false);
  void AddIncludeDirAbsolute(const fs::path &absolute_include_dir,
                             bool glob_headers = false);

  void AddLibDir(const fs::path &relative_lib_dir);
  void AddLibDirAbsolute(const fs::path &absolute_lib_dir);

  // * Libraries
  void AddLibDep(const Target &lib_dep);
  void AddLibDep(const std::string &lib_dep);

  // * Flags
  void AddPreprocessorFlag(const std::string &flag);
  void AddCommonCompileFlag(const std::string &flag);
  void AddPchCompileFlag(const std::string &flag);
  void AddPchObjectFlag(const std::string &flag);
  void AddAsmCompileFlag(const std::string &flag);
  void AddCCompileFlag(const std::string &flag);
  void AddCppCompileFlag(const std::string &flag);
  void AddLinkFlag(const std::string &flag);

  // * Rebuild
  void AddCompileDependency(const fs::path &relative_path);
  void AddCompileDependencyAbsolute(const fs::path &absolute_path);
  void AddLinkDependency(const fs::path &relative_path);
  void AddLinkDependencyAbsolute(const fs::path &absolute_path);

  // TODO, Add more setters

  // Getters (GENERIC)

  // Target state

  // Set during first build or rebuild
  // lock == true after Build is called
  const State &GetState() const { return state_; }
  bool GetBuildState() const { return state_.build; }
  bool GetLockState() const { return state_.lock; }

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
  Target::Type GetType() const { return type_; }
  const fs::path &GetTargetRootDir() const { return env_.target_root_dir; }
  const fs::path &GetTargetBuildDir() const { return env_.target_build_dir; }
  const Config &GetConfig() const { return config_; }

  //
  const internal::fs_unordered_set &GetCurrentSourceFiles() const {
    return storer_.current_source_files.user;
  }
  const internal::fs_unordered_set &GetCurrentHeaderFiles() const {
    return storer_.current_header_files.user;
  }
  const internal::fs_unordered_set &GetCurrentPchFiles() const {
    return storer_.current_pch_files.user;
  }
  const internal::fs_unordered_set &GetTargetLibDeps() const {
    return storer_.current_lib_deps.user;
  }
  const std::unordered_set<std::string> &GetCurrentExternalLibDeps() const {
    return storer_.current_external_lib_deps;
  }
  const internal::fs_unordered_set &GetCurrentIncludeDirs() const {
    return storer_.current_include_dirs;
  }
  const internal::fs_unordered_set &GetCurrentLibDirs() const {
    return storer_.current_lib_dirs;
  }
  const std::unordered_set<std::string> &GetCurrentPreprocessorFlags() const {
    return storer_.current_preprocessor_flags;
  }
  const std::unordered_set<std::string> &GetCurrentCommonCompileFlags() const {
    return storer_.current_common_compile_flags;
  }
  const std::unordered_set<std::string> &GetCurrentPchCompileFlags() const {
    return storer_.current_pch_compile_flags;
  }
  const std::unordered_set<std::string> &GetCurrentPchObjectFlags() const {
    return storer_.current_pch_object_flags;
  }
  const std::unordered_set<std::string> &GetCurrentAsmCompileFlags() const {
    return storer_.current_asm_compile_flags;
  }
  const std::unordered_set<std::string> &GetCurrentCCompileFlags() const {
    return storer_.current_c_compile_flags;
  }
  const std::unordered_set<std::string> &GetCurrentCppCompileFlags() const {
    return storer_.current_cpp_compile_flags;
  }
  const std::unordered_set<std::string> &GetCurrentLinkFlags() const {
    return storer_.current_link_flags;
  }
  const internal::fs_unordered_set &GetCurrentCompileDependencies() const {
    return storer_.current_compile_dependencies.user;
  }
  const internal::fs_unordered_set &GetCurrentLinkDependencies() const {
    return storer_.current_link_dependencies.user;
  }

  // Getters (UnlockedAfterBuild)

  const std::string &GetCompileCommand(const fs::path &source) const {
    UnlockedAfterBuild();
    return compile_object_.GetObjectData(source).command;
  }
  const std::string &GetLinkCommand() const {
    UnlockedAfterBuild();
    return link_target_.GetCommand();
  }

  tf::Taskflow &GetTaskflow() {
    UnlockedAfterBuild();
    return tf_;
  }

  // TODO, Add more getters

private:
  friend class FileExt;
  friend class CompilePch;
  friend class CompileObject;
  friend class LinkTarget;

private:
  void Initialize();

  // Sets lock_ == true
  // NOTE: There is no Unlock function
  void Lock();

  // Expects lock_ == false
  void LockedAfterBuild() const;

  // Expects lock_ == true
  void UnlockedAfterBuild() const;

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

  // Callbacks
  void SourceRemoved();
  void SourceAdded();
  void SourceUpdated();
  void PathRemoved();
  void PathAdded();
  void PathUpdated();

  void DirChanged();
  void FlagChanged();
  void ExternalLibChanged();

  void TaskDeps();

private:
  // Constructor defined
  std::string name_;
  Type type_;
  const Toolchain &toolchain_;
  Config config_;
  Env env_;
  internal::TargetLoader loader_;

  // Friend
  FileExt ext_;
  CompilePch compile_pch_;
  CompileObject compile_object_;
  LinkTarget link_target_;

  // Used for serialization
  internal::TargetStorer storer_;
  State state_;
  Command command_;
  tf::Taskflow tf_;
};

} // namespace buildcc::base

#endif
