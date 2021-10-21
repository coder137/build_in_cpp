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

#include "target/friend/pch.h"

#include "target/path.h"
#include "target/target.h"

#include "env/util.h"

namespace {

constexpr const char *const kCompiler = "compiler";
constexpr const char *const kCompileFlags = "compile_flags";
constexpr const char *const kOutput = "output";
constexpr const char *const kInput = "input";

constexpr const char *const kFormat = R"(// Generated by BuildCC
#ifndef BUILDCC_GENERATED_PCH_H_
#define BUILDCC_GENERATED_PCH_H_

// clang-format off
{aggregated_includes}

#endif
)";

void AggregateToFile(const fs::path &filename,
                     const buildcc::internal::fs_unordered_set &header_files) {
  std::string aggregated_includes;
  for (const auto &hf : header_files) {
    std::string temp = fmt::format("#include \"{}\"\r\n", hf.string());
    aggregated_includes.append(temp);
  }

  buildcc::Command command;
  std::string constructed_output = command.Construct(
      kFormat, {
                   {"aggregated_includes", aggregated_includes},
               });
  bool success = buildcc::env::SaveFile(filename.string().c_str(),
                                        constructed_output, false);
  buildcc::env::assert_fatal(success, "Could not save pch file");
}

} // namespace

namespace buildcc::base {

// PUBLIC

void Pch::CacheCompileCommand() { command_ = ConstructCompileCommand(); }

// PRIVATE

void Pch::BuildCompile() {
  PreCompile();

  const auto &loader = target_.loader_;

  if (!loader.IsLoaded()) {
    target_.dirty_ = true;
  } else {
    target_.RecheckFlags(loader.GetLoadedPreprocessorFlags(),
                         target_.GetCurrentPreprocessorFlags());
    target_.RecheckFlags(loader.GetLoadedCommonCompileFlags(),
                         target_.GetCurrentCommonCompileFlags());
    target_.RecheckFlags(loader.GetLoadedCCompileFlags(),
                         target_.GetCurrentCCompileFlags());
    target_.RecheckFlags(loader.GetLoadedCppCompileFlags(),
                         target_.GetCurrentCppCompileFlags());
    target_.RecheckDirs(loader.GetLoadedIncludeDirs(),
                        target_.GetCurrentIncludeDirs());
    target_.RecheckPaths(loader.GetLoadedHeaders(),
                         target_.storer_.current_header_files.internal);

    target_.RecheckFlags(loader.GetLoadedPchCompileFlags(),
                         target_.GetCurrentPchCompileFlags());
    target_.RecheckPaths(loader.GetLoadedPchs(),
                         target_.storer_.current_pch_files.internal);
  }

  if (target_.dirty_) {
    AggregateToFile(header_path_, target_.GetCurrentPchFiles());
    bool success = Command::Execute(command_);
    env::assert_fatal(success, "Failed to compile pch");
  }
}

fs::path Pch::ConstructHeaderPath() const {
  return target_.target_build_dir_ /
         fmt::format("buildcc_pch{}", target_.GetConfig().pch_header_ext);
}

fs::path Pch::ConstructCompilePath() const {
  return ConstructHeaderPath().replace_extension(
      fmt::format("{}{}", target_.GetConfig().pch_header_ext,
                  target_.GetConfig().pch_compile_ext));
}

std::string Pch::ConstructCompileCommand() const {
  const std::string compiler = target_.GetState().contains_cpp_src
                                   ? target_.GetToolchain().GetCppCompiler()
                                   : target_.GetToolchain().GetCCompiler();
  const FileExt::Type file_ext_type = target_.GetState().contains_cpp_src
                                          ? FileExt::Type::Cpp
                                          : FileExt::Type::C;
  const std::string compile_flags =
      target_.ext_.GetCompileFlags(file_ext_type).value_or("");
  const std::string pch_compile_path =
      internal::Path::CreateNewPath(compile_path_).GetPathAsString();
  const std::string pch_header_path =
      internal::Path::CreateNewPath(header_path_).GetPathAsString();
  return target_.command_.Construct(target_.config_.pch_command,
                                    {
                                        {kCompiler, compiler},
                                        {kCompileFlags, compile_flags},
                                        {kOutput, pch_compile_path},
                                        {kInput, pch_header_path},
                                    });
}

void Pch::PreCompile() {
  target_.storer_.current_header_files.Convert();

  target_.storer_.current_pch_files.Convert();
}

} // namespace buildcc::base
