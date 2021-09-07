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

#include "target/util.h"

#include "env/assert_fatal.h"

#include "fmt/format.h"

namespace buildcc::base {

// * Load
// TODO, Verify things that cannot be changed
// * Compile
// Include directories dependencies
// * Link
// Library dependencies
void Target::Build() {
  env::log_trace(name_, __FUNCTION__);

  // Taskflow updates
  tf_.name(fmt::format("[{}] {}", toolchain_.GetName(), name_));

  // TODO, Optimize these
  aggregated_c_compile_flags_ = internal::aggregate(current_c_compile_flags_);
  aggregated_cpp_compile_flags_ =
      internal::aggregate(current_cpp_compile_flags_);

  command_.AddDefaultArguments({
      {"include_dirs", internal::aggregate_with_prefix(prefix_include_dir_,
                                                       current_include_dirs_)},
      {"lib_dirs",
       internal::aggregate_with_prefix(prefix_lib_dir_, current_lib_dirs_)},

      {"preprocessor_flags", internal::aggregate(current_preprocessor_flags_)},
      {"common_compile_flags",
       internal::aggregate(current_common_compile_flags_)},
      {"link_flags", internal::aggregate(current_link_flags_)},

      // Toolchain executables here
      {"asm_compiler", toolchain_.GetAsmCompiler()},
      {"c_compiler", toolchain_.GetCCompiler()},
      {"cpp_compiler", toolchain_.GetCppCompiler()},
      {"archiver", toolchain_.GetArchiver()},
      {"linker", toolchain_.GetLinker()},
  });

  CompileTask();
  LinkTask();
}

std::string Target::LinkCommand() const {

  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(GetCompiledSources());

  const std::string output_target =
      internal::Path::CreateNewPath(GetTargetPath()).GetPathAsString();

  return command_.Construct(
      link_command_,
      {
          {"output", output_target},
          {"compiled_sources", aggregated_compiled_sources},
          {"lib_deps",
           fmt::format("{} {}", internal::aggregate(current_external_lib_deps_),
                       internal::aggregate(current_lib_deps_.internal))},
      });

  const bool success = command_.ConstructAndExecute(
      link_command_,
      {
          {"output", output_target},
          {"compiled_sources", aggregated_compiled_sources},
          {"lib_deps",
           fmt::format("{} {}", internal::aggregate(current_external_lib_deps_),
                       internal::aggregate(current_lib_deps_.internal))},
      });
  env::assert_fatal(success, fmt::format("Compilation failed for: {}", name_));
  return "";
}

//

void Target::ConvertForCompile() {
  // Convert user_source_files to current_source_files
  for (const auto &user_sf : current_source_files_.user) {
    current_source_files_.internal.emplace(
        buildcc::internal::Path::CreateExistingPath(user_sf));
  }

  // Convert user_header_files to current_header_files
  for (const auto &user_hf : current_header_files_.user) {
    current_header_files_.internal.emplace(
        buildcc::internal::Path::CreateExistingPath(user_hf));
  }

  for (const auto &user_cd : current_compile_dependencies_.user) {
    current_compile_dependencies_.internal.emplace(
        internal::Path::CreateExistingPath(user_cd));
  }
}

void Target::ConvertForLink() {
  std::for_each(
      current_lib_deps_.user.cbegin(), current_lib_deps_.user.cend(),
      [this](const Target *target) {
        current_lib_deps_.internal.emplace(
            internal::Path::CreateExistingPath(target->GetTargetPath()));
      });

  for (const auto &user_ld : current_link_dependencies_.user) {
    current_link_dependencies_.internal.emplace(
        internal::Path::CreateExistingPath(user_ld));
  }
}

void Target::BuildCompile(std::vector<fs::path> &compile_sources,
                          std::vector<fs::path> &dummy_sources) {
  const bool is_loaded = loader_.Load();
  if (!is_loaded) {
    CompileSources(compile_sources);
    dirty_ = true;
  } else {
    // * Completely compile sources if any of the following change
    // TODO, Toolchain, ASM, C, C++ compiler related to a particular name
    RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
                 current_preprocessor_flags_);
    RecheckFlags(loader_.GetLoadedCommonCompileFlags(),
                 current_common_compile_flags_);
    RecheckFlags(loader_.GetLoadedCCompileFlags(), current_c_compile_flags_);
    RecheckFlags(loader_.GetLoadedCppCompileFlags(),
                 current_cpp_compile_flags_);
    RecheckDirs(loader_.GetLoadedIncludeDirs(), current_include_dirs_);
    RecheckPaths(loader_.GetLoadedHeaders(), current_header_files_.internal);
    RecheckPaths(loader_.GetLoadedCompileDependencies(),
                 current_compile_dependencies_.internal);

    // * Compile sources
    if (dirty_) {
      CompileSources(compile_sources);
    } else {
      RecompileSources(compile_sources, dummy_sources);
    }
  }
}

void Target::BuildCompileGenerator() {
  compile_generator_.AddPregenerateCb([&]() { ConvertForCompile(); });
  compile_generator_.AddCustomRegenerateCb(
      [&](const internal::geninfo_unordered_map &previous_info,
          const internal::geninfo_unordered_map &current_info,
          std::vector<const internal::GenInfo *> &output_generated_files,
          std::vector<const internal::GenInfo *>
              &output_dummy_generated_files) {
        const bool is_loaded = loader_.Load();
        (void)is_loaded;

        RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
                     current_preprocessor_flags_);
        RecheckFlags(loader_.GetLoadedCommonCompileFlags(),
                     current_common_compile_flags_);
        RecheckFlags(loader_.GetLoadedCCompileFlags(),
                     current_c_compile_flags_);
        RecheckFlags(loader_.GetLoadedCppCompileFlags(),
                     current_cpp_compile_flags_);
        RecheckDirs(loader_.GetLoadedIncludeDirs(), current_include_dirs_);
        RecheckPaths(loader_.GetLoadedHeaders(),
                     current_header_files_.internal);
        RecheckPaths(loader_.GetLoadedCompileDependencies(),
                     current_compile_dependencies_.internal);

        if (dirty_) {
          std::transform(current_info.begin(), current_info.end(),
                         std::back_inserter(output_generated_files),
                         [](const auto &ci) -> const internal::GenInfo * {
                           return &(ci.second);
                         });
        } else {
          bool build = false;

          // previous_info has more items than current_info
          for (const auto &pi : previous_info) {
            if (current_info.find(pi.first) == current_info.end()) {
              SourceRemoved();
              build = true;
              break;
            }
          }

          // Check all files individually
          for (const auto &ci : current_info) {
            if (previous_info.find(ci.first) == previous_info.end()) {
              // current_info has more items than
              // previous_info
              SourceAdded();
              dirty_ = true;
            } else {
              const internal::GenInfo &loaded_geninfo =
                  previous_info.at(ci.first);
              BuilderInterface::RecheckPaths(loaded_geninfo.inputs.internal,
                                             ci.second.inputs.internal);
              RecheckChanged(loaded_geninfo.outputs, ci.second.outputs);
              RecheckChanged(loaded_geninfo.commands, ci.second.commands);
              if (dirty_) {
                SourceUpdated();
              }
            }

            if (dirty_) {
              output_generated_files.push_back(&(ci.second));
              build = true;
            } else {
              output_dummy_generated_files.push_back(&(ci.second));
            }
            dirty_ = false;
          }
          dirty_ = build;
        }

        return dirty_;
      });
  compile_generator_.AddPostgenerateCb([&]() { dirty_ = true; });

  for (const auto &cof : current_object_files_) {
    std::string name = fs::path(cof.first)
                           .lexically_relative(env::get_project_root_dir())
                           .string();
    compile_generator_.AddGenInfo(name, {cof.first}, {cof.second.GetPathname()},
                                  {CompileCommand(cof.first)}, true);
  }
}

void Target::BuildLink() {
  // * Completely rebuild target / link if any of the following change
  // Target compiled source files either during Compile / Recompile
  // Target library dependencies
  RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
  RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
  RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                     current_external_lib_deps_);
  RecheckPaths(loader_.GetLoadedLinkDependencies(),
               current_link_dependencies_.internal);
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_.internal);

  if (dirty_) {
    LinkTarget();
    Store();
  }
}

void Target::BuildLinkGenerator() {
  link_generator_.AddPregenerateCb([&]() { ConvertForLink(); });
  link_generator_.AddCustomRegenerateCb(
      [&](const internal::geninfo_unordered_map &previous_info,
          const internal::geninfo_unordered_map &current_info,
          std::vector<const internal::GenInfo *> &output_generated_files,
          std::vector<const internal::GenInfo *>
              &output_dummy_generated_files) {
        (void)previous_info;
        // * Completely rebuild target / link if any of the following change
        // Target compiled source files either during Compile / Recompile
        // Target library dependencies
        RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
        RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
        RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                           current_external_lib_deps_);
        RecheckPaths(loader_.GetLoadedLinkDependencies(),
                     current_link_dependencies_.internal);
        RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_.internal);

        if (dirty_) {
          std::transform(current_info.begin(), current_info.end(),
                         std::back_inserter(output_generated_files),
                         [](const auto &ci) -> const internal::GenInfo * {
                           return &(ci.second);
                         });
        } else {
          std::transform(current_info.begin(), current_info.end(),
                         std::back_inserter(output_dummy_generated_files),
                         [](const auto &ci) -> const internal::GenInfo * {
                           return &(ci.second);
                         });
        }

        return dirty_;
      });
  link_generator_.AddPostgenerateCb([&]() {
    Store();
    dirty_ = true;
    build_ = true;
  });
  std::string name =
      GetTargetPath().lexically_relative(env::get_project_build_dir()).string();
  link_generator_.AddGenInfo(name, {}, {GetTargetPath()}, {LinkCommand()},
                             false);
}

void Target::LinkTarget() {
  const bool success = Command::Execute(LinkCommand());
  env::assert_fatal(success, fmt::format("Compilation failed for: {}", name_));
}

} // namespace buildcc::base
