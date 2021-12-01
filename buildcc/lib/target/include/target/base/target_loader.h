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

#ifndef TARGET_BASE_TARGET_LOADER_H_
#define TARGET_BASE_TARGET_LOADER_H_

#include "target/interface/loader_interface.h"

#include <string>
#include <unordered_set>

#include "fmt/format.h"

#include "target/common/path.h"

namespace buildcc::internal {

class TargetLoader : public LoaderInterface {
public:
  explicit TargetLoader(const std::string &name, const fs::path &relative_path)
      : name_(name), relative_path_(relative_path) {
    binary_path_ = relative_path / fmt::format("{}.bin", name);
    Initialize();
  }

  TargetLoader(const TargetLoader &loader) = delete;

public:
  bool Load() override;

  // Getters
  const path_unordered_set &GetLoadedSources() const noexcept {
    return loaded_sources_;
  }
  const path_unordered_set &GetLoadedHeaders() const noexcept {
    return loaded_headers_;
  }
  const path_unordered_set &GetLoadedPchs() const noexcept {
    return loaded_pchs_;
  }
  const path_unordered_set &GetLoadedLibDeps() const noexcept {
    return loaded_lib_deps_;
  }
  const std::unordered_set<std::string> &
  GetLoadedExternalLibDeps() const noexcept {
    return loaded_external_lib_dirs_;
  }

  const fs_unordered_set &GetLoadedIncludeDirs() const noexcept {
    return loaded_include_dirs_;
  }
  const fs_unordered_set &GetLoadedLibDirs() const noexcept {
    return loaded_lib_dirs_;
  }
  const std::unordered_set<std::string> &
  GetLoadedPreprocessorFlags() const noexcept {
    return loaded_preprocessor_flags_;
  }
  const std::unordered_set<std::string> &
  GetLoadedCommonCompileFlags() const noexcept {
    return loaded_common_compile_flags_;
  }
  const std::unordered_set<std::string> &
  GetLoadedPchCompileFlags() const noexcept {
    return loaded_pch_compile_flags_;
  }
  const std::unordered_set<std::string> &
  GetLoadedPchObjectFlags() const noexcept {
    return loaded_pch_object_flags_;
  }
  const std::unordered_set<std::string> &
  GetLoadedAsmCompileFlags() const noexcept {
    return loaded_asm_compile_flags_;
  }
  const std::unordered_set<std::string> &
  GetLoadedCCompileFlags() const noexcept {
    return loaded_c_compile_flags_;
  }
  const std::unordered_set<std::string> &
  GetLoadedCppCompileFlags() const noexcept {
    return loaded_cpp_compile_flags_;
  }
  const std::unordered_set<std::string> &GetLoadedLinkFlags() const noexcept {
    return loaded_link_flags_;
  }

  const path_unordered_set &GetLoadedCompileDependencies() const noexcept {
    return loaded_compile_dependencies_;
  }
  const path_unordered_set &GetLoadedLinkDependencies() const noexcept {
    return loaded_link_dependencies_;
  }

  bool GetLoadedTargetLinked() const noexcept { return loaded_target_linked_; }

private:
  void Initialize();

private:
  std::string name_;
  fs::path relative_path_;

  path_unordered_set loaded_sources_;
  path_unordered_set loaded_headers_;
  path_unordered_set loaded_pchs_;
  path_unordered_set loaded_lib_deps_;

  std::unordered_set<std::string> loaded_external_lib_dirs_;

  fs_unordered_set loaded_include_dirs_;
  fs_unordered_set loaded_lib_dirs_;

  std::unordered_set<std::string> loaded_preprocessor_flags_;
  std::unordered_set<std::string> loaded_common_compile_flags_;
  std::unordered_set<std::string> loaded_pch_compile_flags_;
  std::unordered_set<std::string> loaded_pch_object_flags_;
  std::unordered_set<std::string> loaded_asm_compile_flags_;
  std::unordered_set<std::string> loaded_c_compile_flags_;
  std::unordered_set<std::string> loaded_cpp_compile_flags_;
  std::unordered_set<std::string> loaded_link_flags_;

  path_unordered_set loaded_compile_dependencies_;
  path_unordered_set loaded_link_dependencies_;

  bool loaded_target_linked_{false};
};

} // namespace buildcc::internal

#endif
