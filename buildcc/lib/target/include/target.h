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

#ifndef TARGET_INCLUDE_TARGET_H_
#define TARGET_INCLUDE_TARGET_H_

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Internal
#include "internal/fbs_loader.h"
#include "internal/path.h"

// Buildcc
#include "toolchain.h"

// Env
#include "env.h"

// Third Party
#include "taskflow/taskflow.hpp"

namespace buildcc::base {

namespace fs = std::filesystem;

enum class FileExtType {
  Asm,
  C,
  Cpp,
  Header,
  Invalid,
};

enum class TargetType {
  Executable,
  StaticLibrary,
  DynamicLibrary,
};

class Target {
public:
  explicit Target(const std::string &name, TargetType type,
                  const Toolchain &toolchain,
                  const fs::path &target_path_relative_to_root)
      : name_(name), type_(type), toolchain_(toolchain),
        target_root_source_dir_(env::get_project_root_dir() /
                                target_path_relative_to_root),
        target_intermediate_dir_(fs::path(env::get_project_build_dir()) /
                                 toolchain.GetName() / name),
        loader_(name, target_intermediate_dir_) {
    Initialize();
  }

  Target(const Target &target) = delete;

  // Builders
  void Build();

  // Setters

  // * Sources
  void AddSource(const fs::path &relative_filename);
  void AddSource(const fs::path &relative_filename,
                 const fs::path &relative_to_target_path);
  void GlobSources(const fs::path &relative_to_target_path);

  // Use these APIs for out of project root builds
  // Manually specify input and output
  void AddSourceAbsolute(const fs::path &absolute_input_filepath,
                         const fs::path &absolute_output_filepath);
  void GlobSourcesAbsolute(const fs::path &absolute_input_path,
                           const fs::path &absolute_output_path);

  // * Headers
  void AddHeader(const std::string &relative_filename);
  void AddHeader(const std::string &relative_filename,
                 const fs::path &relative_to_target_path);
  void AddHeaderAbsolute(const fs::path &absolute_filepath);

  void GlobHeaders(const fs::path &relative_to_target_path);
  void GlobHeadersAbsolute(const fs::path &absolute_path);

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
  void AddCCompileFlag(const std::string &flag);
  void AddCppCompileFlag(const std::string &flag);
  void AddLinkFlag(const std::string &flag);

  // TODO, Add more setters

  // Getters
  std::vector<std::string> CompileCommand(const fs::path &current_source) const;

  // TODO, Check if these need to be made const
  tf::Taskflow &GetTaskflow() { return tf_; }
  tf::Task &GetCompileTask() { return compile_task_; }
  tf::Task &GetLinkTask() { return link_task_; }

  fs::path GetTargetPath() const {
    fs::path path = GetTargetIntermediateDir() / GetName();
    path.make_preferred();
    return path;
  }
  fs::path GetBinaryPath() const { return loader_.GetBinaryPath(); }

  // Const references
  const std::string &GetName() const { return name_; }
  const Toolchain &GetToolchain() const { return toolchain_; }
  base::TargetType GetTargetType() const { return type_; }

  const fs::path &GetTargetRootDir() const { return target_root_source_dir_; }
  const fs::path &GetTargetIntermediateDir() const {
    return target_intermediate_dir_;
  }

  // TODO, Consider returning `std::vector<std::string>` OR
  // `std::vector<fs::path>` for these getters
  // These APIs are meant to be consumed by users
  const internal::path_unordered_set &GetCurrentSourceFiles() const {
    return current_source_files_;
  }
  const internal::path_unordered_set &GetCurrentHeaderFiles() const {
    return current_header_files_;
  }

  bool FirstBuild() const { return first_build_; }
  bool Rebuild() const { return rebuild_; }

  // TODO, Add more getters

public:
  std::string prefix_include_dir_{"-I"};
  std::string prefix_lib_dir_{"-L"};
  std::unordered_set<std::string> valid_c_ext_{".c"};
  std::unordered_set<std::string> valid_cpp_ext_{".cpp", ".cxx", ".cc"};
  std::unordered_set<std::string> valid_asm_ext_{".s", ".S", ".asm"};
  std::unordered_set<std::string> valid_header_ext_{".h", ".hpp"};

protected:
  // Getters
  FileExtType GetFileExtType(const fs::path &filepath) const;
  bool IsValidSource(const fs::path &sourcepath) const;
  bool IsValidHeader(const fs::path &headerpath) const;

  const std::string &GetCompiler(const fs::path &source) const;

  const internal::Path &GetCompiledSourcePath(const fs::path &source) const;
  internal::path_unordered_set GetCompiledSources() const;

private:
  void Initialize();

  // Build
  void BuildCompile();
  void BuildRecompile();

  // Compile
  void CompileSources();
  void RecompileSources();

  void CompileSource(const fs::path &current_source) const;

  // * Virtual
  // PreCompile();
  // Compile();
  // PostCompile();
  virtual std::vector<std::string>
  CompileCommand(const std::string &input_source,
                 const std::string &output_source, const std::string &compiler,
                 const std::string &aggregated_preprocessor_flags,
                 const std::string &aggregated_compile_flags,
                 const std::string &aggregated_include_dirs) const;

  // Recompilation checks
  void RecheckPaths(const internal::path_unordered_set &previous_path,
                    const internal::path_unordered_set &current_path);

  void RecheckDirs(const std::unordered_set<std::string> &previous_dirs,
                   const std::unordered_set<std::string> &current_dirs);
  void RecheckFlags(const std::unordered_set<std::string> &previous_flags,
                    const std::unordered_set<std::string> &current_flags);
  void RecheckExternalLib(
      const std::unordered_set<std::string> &previous_external_libs,
      const std::unordered_set<std::string> &current_external_libs);

  // Helper function
  void RecheckChanged(const std::unordered_set<std::string> &previous,
                      const std::unordered_set<std::string> &current,
                      std::function<void(void)> callback);

  // Tasks
  void CompileTargetTask(const std::vector<fs::path> &&compile_sources,
                         const std::vector<fs::path> &&dummy_compile_sources);

  void LinkTargetTask(const bool link);

  // * Virtual
  // PreLink();
  // Link();
  // PostLink();

  void LinkTarget();

  // TODO, Add Link library paths
  virtual std::vector<std::string>
  Link(const std::string &output_target,
       const std::string &aggregated_link_flags,
       const std::string &aggregated_compiled_sources,
       const std::string &aggregated_lib_dirs,
       const std::string &aggregated_lib_deps) const;

  // Fbs
  bool Store();

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

private:
  // Constructor defined
  std::string name_;
  TargetType type_;
  const Toolchain &toolchain_;
  fs::path target_root_source_dir_;
  fs::path target_intermediate_dir_;

  // Internal
  internal::path_unordered_set current_source_files_;
  // NOTE, Always store the absolute source path -> absolute compiled source
  // path here
  std::unordered_map<fs::path::string_type, internal::Path>
      current_object_files_;

  internal::path_unordered_set current_header_files_;
  internal::path_unordered_set current_lib_deps_;

  // TODO, Change these to PATHS
  std::unordered_set<std::string> current_include_dirs_;
  std::unordered_set<std::string> current_lib_dirs_;

  std::unordered_set<std::string> current_external_lib_deps_;

  std::unordered_set<std::string> current_preprocessor_flags_;
  std::unordered_set<std::string> current_c_compile_flags_;
  std::unordered_set<std::string> current_cpp_compile_flags_;
  std::unordered_set<std::string> current_link_flags_;

  // TODO, Make appending to this more efficient
  // TODO, Might not need to be persistent
  // TODO, aggregates might not need to exist, check `std::insert` APIs over
  // vector and unordered_set
  std::string aggregated_include_dirs_;
  std::string aggregated_lib_dirs_;
  // NOTE, This contains current_external_lib_deps_ + current_lib_deps_
  std::string aggregated_lib_deps_;

  std::string aggregated_preprocessor_flags_;
  std::string aggregated_c_compile_flags_;
  std::string aggregated_cpp_compile_flags_;
  std::string aggregated_link_flags_;

  // TODO, Add more internal variables

  internal::FbsLoader loader_;
  bool dirty_ = false;

  // Build states
  bool first_build_ = false;
  bool rebuild_ = false;

  static constexpr const char *const kCompileTaskName = "Compile";
  static constexpr const char *const kLinkTaskName = "Link";

  tf::Taskflow tf_;
  tf::Task compile_task_;
  tf::Task link_task_;
};

} // namespace buildcc::base

#endif
