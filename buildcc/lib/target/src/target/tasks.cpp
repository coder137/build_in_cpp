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

#include <algorithm>

#include "env/assert_fatal.h"
#include "env/logging.h"

#include "target/util.h"

#include "fmt/format.h"

namespace {

constexpr const char *const kCompileTaskName = "Compile";
constexpr const char *const kLinkTaskName = "Link";

} // namespace

namespace buildcc::base {

void Target::CompileTask() {
  env::log_trace(name_, __FUNCTION__);

  compile_task_ = tf_.emplace([&](tf::Subflow &subflow) {
    ConvertForCompile();

    std::vector<fs::path> source_files;
    std::vector<fs::path> dummy_source_files;

    if (!loader_.IsLoaded()) {
      CompileSources(source_files);
      dirty_ = true;
    } else {
      RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
                   current_preprocessor_flags_);
      RecheckFlags(loader_.GetLoadedCommonCompileFlags(),
                   current_common_compile_flags_);
      RecheckFlags(loader_.GetLoadedAsmCompileFlags(),
                   current_asm_compile_flags_);
      RecheckFlags(loader_.GetLoadedCCompileFlags(), current_c_compile_flags_);
      RecheckFlags(loader_.GetLoadedCppCompileFlags(),
                   current_cpp_compile_flags_);
      RecheckDirs(loader_.GetLoadedIncludeDirs(), current_include_dirs_);
      RecheckPaths(loader_.GetLoadedHeaders(), current_header_files_.internal);
      RecheckPaths(loader_.GetLoadedCompileDependencies(),
                   current_compile_dependencies_.internal);

      if (dirty_) {
        CompileSources(source_files);
      } else {
        RecompileSources(source_files, dummy_source_files);
      }
    }

    for (const auto &s : source_files) {
      std::string name =
          s.lexically_relative(env::get_project_root_dir()).string();
      std::replace(name.begin(), name.end(), '\\', '/');
      (void)subflow
          .emplace([this, s]() {
            bool success = Command::Execute(CompileCommand(s));
            env::assert_fatal(success, "Could not compile source");
          })
          .name(name);
    }

    for (const auto &ds : dummy_source_files) {
      std::string name =
          ds.lexically_relative(env::get_project_root_dir()).string();
      std::replace(name.begin(), name.end(), '\\', '/');
      subflow.placeholder().name(name);
    }
  });
  compile_task_.name(kCompileTaskName);
}

void Target::LinkTask() {
  env::log_trace(name_, __FUNCTION__);

  link_task_ = tf_.emplace([&]() {
    ConvertForLink();

    RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
    RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
    RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                       current_external_lib_deps_);
    RecheckPaths(loader_.GetLoadedLinkDependencies(),
                 current_link_dependencies_.internal);
    RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_.internal);

    if (dirty_) {
      bool success = Command::Execute(LinkCommand());
      env::assert_fatal(success, "Failed to link target");
      Store();
    }

    build_ = true;
  });

  link_task_.name(kLinkTaskName);
  link_task_.succeed(compile_task_);
}

} // namespace buildcc::base
