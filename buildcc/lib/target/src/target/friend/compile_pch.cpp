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

#include "target/friend/compile_pch.h"

#include "schema/path.h"
#include "target/target.h"

#include "env/util.h"

namespace {

constexpr const char *const kCompiler = "compiler";
constexpr const char *const kCompileFlags = "compile_flags";
constexpr const char *const kOutput = "output";
constexpr const char *const kInput = "input";
constexpr const char *const kInputSource = "input_source";

constexpr const char *const kFormat = R"(// Generated by BuildCC
#ifndef BUILDCC_GENERATED_PCH_H_
#define BUILDCC_GENERATED_PCH_H_

// clang-format off
{aggregated_includes}

#endif
)";

void AggregateToFile(const fs::path &filename,
                     const buildcc::fs_unordered_set &header_files) {
  std::string aggregated_includes;
  for (const auto &hf : header_files) {
    std::string temp = fmt::format("#include \"{}\"\r\n", hf);
    aggregated_includes.append(temp);
  }

  buildcc::env::Command command;
  std::string constructed_output = command.Construct(
      kFormat, {
                   {"aggregated_includes", aggregated_includes},
               });
  bool success = buildcc::env::save_file(
      buildcc::path_as_string(filename).c_str(), constructed_output, false);
  buildcc::env::assert_throw(success, "Could not save pch file");
}

} // namespace

namespace buildcc::internal {

// PUBLIC

void CompilePch::CacheCompileCommand() {
  source_path_ = ConstructSourcePath(target_.GetState().contains_cpp);
  command_ = ConstructCompileCommand();
}

// PRIVATE

void CompilePch::BuildCompile() {
  PreCompile();

  const auto &serialization = target_.serialization_;
  const auto &load_target_schema = serialization.GetLoad();
  const auto &user_target_schema = target_.user_;

  if (!serialization.IsLoaded()) {
    target_.dirty_ = true;
  } else {
    target_.RecheckFlags(load_target_schema.preprocessor_flags,
                         user_target_schema.preprocessor_flags);
    target_.RecheckFlags(load_target_schema.common_compile_flags,
                         user_target_schema.common_compile_flags);
    target_.RecheckFlags(load_target_schema.c_compile_flags,
                         user_target_schema.c_compile_flags);
    target_.RecheckFlags(load_target_schema.cpp_compile_flags,
                         user_target_schema.cpp_compile_flags);
    target_.RecheckDirs(load_target_schema.include_dirs,
                        user_target_schema.include_dirs);
    target_.RecheckPaths(load_target_schema.internal_headers,
                         user_target_schema.internal_headers);

    target_.RecheckFlags(load_target_schema.pch_compile_flags,
                         user_target_schema.pch_compile_flags);
    target_.RecheckPaths(load_target_schema.internal_pchs,
                         user_target_schema.internal_pchs);
    if (!load_target_schema.pch_compiled) {
      target_.dirty_ = true;
    }
  }

  if (target_.dirty_) {
    AggregateToFile(header_path_, target_.GetPchFiles());
    if (!fs::exists(source_path_)) {
      const std::string p = fmt::format("{}", source_path_);
      const bool save =
          env::save_file(p.c_str(), {"//Generated by BuildCC"}, false);
      env::assert_throw(save, fmt::format("Could not save {}", p));
    }
    bool success = env::Command::Execute(command_);
    env::assert_throw(success, "Failed to compile pch");
  }
}

fs::path CompilePch::ConstructHeaderPath() const {
  return target_.GetTargetBuildDir() /
         fmt::format("buildcc_pch{}", target_.GetConfig().pch_header_ext);
}

fs::path CompilePch::ConstructCompilePath() const {
  return ConstructHeaderPath().replace_extension(
      fmt::format("{}{}", target_.GetConfig().pch_header_ext,
                  target_.GetConfig().pch_compile_ext));
}

fs::path CompilePch::ConstructSourcePath(bool has_cpp) const {
  return ConstructHeaderPath().replace_extension(
      fmt::format("{}", has_cpp ? ".cpp" : ".c"));
}

fs::path CompilePch::ConstructObjectPath() const {
  return ConstructHeaderPath().replace_extension(
      fmt::format("{}", target_.GetConfig().obj_ext));
}

std::string CompilePch::ConstructCompileCommand() const {
  std::string compiler = target_.GetState().contains_cpp
                             ? target_.GetToolchain().GetCppCompiler()
                             : target_.GetToolchain().GetCCompiler();
  compiler = fmt::format("{}", fs::path(compiler));
  const TargetFileExt file_ext_type =
      target_.GetState().contains_cpp ? TargetFileExt::Cpp : TargetFileExt::C;
  const std::string compile_flags =
      target_.SelectCompileFlags(file_ext_type).value_or("");
  const std::string pch_compile_path = fmt::format("{}", compile_path_);
  const std::string pch_header_path = fmt::format("{}", header_path_);
  const std::string pch_source_path = fmt::format("{}", source_path_);
  return target_.command_.Construct(target_.config_.pch_command,
                                    {
                                        {kCompiler, compiler},
                                        {kCompileFlags, compile_flags},
                                        {kOutput, pch_compile_path},
                                        {kInput, pch_header_path},
                                        {kInputSource, pch_source_path},
                                    });
}

void CompilePch::PreCompile() {
  auto &target_user_schema = target_.user_;

  target_user_schema.internal_headers =
      internal::path_schema_convert(target_user_schema.headers);

  target_user_schema.internal_pchs =
      internal::path_schema_convert(target_user_schema.pchs);
}

} // namespace buildcc::internal
