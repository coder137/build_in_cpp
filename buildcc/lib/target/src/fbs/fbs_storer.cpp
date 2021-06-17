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

#include "target.h"

#include <algorithm>
#include <filesystem>

#include "logging.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

namespace fbs = schema::internal;

namespace {

fbs::TargetType get_fbs_target_type(buildcc::base::TargetType type) {
  fbs::TargetType target_type = fbs::TargetType_Executable;
  switch (type) {
  case buildcc::base::TargetType::Executable:
    break;
  case buildcc::base::TargetType::StaticLibrary:
    target_type = fbs::TargetType::TargetType_StaticLibrary;
    break;
  case buildcc::base::TargetType::DynamicLibrary:
    target_type = fbs::TargetType_DynamicLibrary;
    break;
  }
  return target_type;
}

std::vector<flatbuffers::Offset<fbs::Path>>
get_fbs_vector_path(flatbuffers::FlatBufferBuilder &builder,
                    const buildcc::internal::path_unordered_set &pathlist) {
  std::vector<flatbuffers::Offset<fbs::Path>> paths;
  for (const auto &p : pathlist) {
    auto fbs_file = fbs::CreatePathDirect(
        builder, p.GetPathname().string().c_str(), p.GetLastWriteTimestamp());
    paths.push_back(fbs_file);
  }
  return paths;
}

std::vector<flatbuffers::Offset<flatbuffers::String>>
get_fbs_vector_string(flatbuffers::FlatBufferBuilder &builder,
                      const std::unordered_set<std::string> &strlist) {
  std::vector<flatbuffers::Offset<flatbuffers::String>> strs;
  std::transform(
      strlist.begin(), strlist.end(), std::back_inserter(strs),
      [&](const std::string &str) -> flatbuffers::Offset<flatbuffers::String> {
        return builder.CreateString(str);
      });
  return strs;
}

} // namespace

namespace buildcc::base {

bool Target::Store() {
  env::log_trace(name_, __FUNCTION__);

  flatbuffers::FlatBufferBuilder builder;

  auto fbs_target_type = get_fbs_target_type(type_);

  auto fbs_source_files = get_fbs_vector_path(builder, current_source_files_);
  auto fbs_header_files = get_fbs_vector_path(builder, current_header_files_);
  auto fbs_lib_deps = get_fbs_vector_path(builder, current_lib_deps_);

  auto fbs_external_lib_deps =
      get_fbs_vector_string(builder, current_external_lib_deps_);

  auto fbs_include_dirs = get_fbs_vector_string(builder, current_include_dirs_);
  auto fbs_lib_dirs = get_fbs_vector_string(builder, current_lib_dirs_);

  auto fbs_preprocessor_flags =
      get_fbs_vector_string(builder, current_preprocessor_flags_);
  auto fbs_c_compiler_flags =
      get_fbs_vector_string(builder, current_c_compile_flags_);
  auto fbs_cpp_compiler_flags =
      get_fbs_vector_string(builder, current_cpp_compile_flags_);
  auto fbs_link_flags = get_fbs_vector_string(builder, current_link_flags_);

  auto fbs_target = fbs::CreateTargetDirect(
      builder, name_.c_str(), fbs_target_type, &fbs_source_files,
      &fbs_header_files, &fbs_lib_deps, &fbs_external_lib_deps,
      &fbs_include_dirs, &fbs_lib_dirs, &fbs_preprocessor_flags,
      &fbs_c_compiler_flags, &fbs_cpp_compiler_flags, &fbs_link_flags);
  fbs::FinishTargetBuffer(builder, fbs_target);

  auto file_path = GetBinaryPath();
  return flatbuffers::SaveFile(file_path.string().c_str(),
                               (const char *)builder.GetBufferPointer(),
                               builder.GetSize(), true);
}

} // namespace buildcc::base
