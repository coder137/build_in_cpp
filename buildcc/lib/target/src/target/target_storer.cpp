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
#include <filesystem>

#include "env/logging.h"
#include "env/util.h"

#include "target/private/schema_util.h"
#include "target_generated.h"

namespace fbs = schema::internal;

namespace {

fbs::TargetType CreateFbsTargetType(buildcc::base::TargetType type) {
  return (fbs::TargetType)type;
}

} // namespace

namespace buildcc::base {

bool Target::Store() {
  env::log_trace(name_, __FUNCTION__);

  flatbuffers::FlatBufferBuilder builder;

  auto fbs_target_type = CreateFbsTargetType(type_);

  auto fbs_source_files =
      internal::CreateFbsVectorPath(builder, current_source_files_.internal);
  auto fbs_header_files =
      internal::CreateFbsVectorPath(builder, current_header_files_.internal);
  auto fbs_lib_deps = internal::CreateFbsVectorPath(builder, current_lib_deps_);

  auto fbs_external_lib_deps =
      internal::CreateFbsVectorString(builder, current_external_lib_deps_);

  auto fbs_include_dirs =
      internal::CreateFbsVectorString(builder, current_include_dirs_);
  auto fbs_lib_dirs =
      internal::CreateFbsVectorString(builder, current_lib_dirs_);

  auto fbs_preprocessor_flags =
      internal::CreateFbsVectorString(builder, current_preprocessor_flags_);
  auto fbs_common_compile_flags =
      internal::CreateFbsVectorString(builder, current_common_compile_flags_);
  auto fbs_c_compile_flags =
      internal::CreateFbsVectorString(builder, current_c_compile_flags_);
  auto fbs_cpp_compile_flags =
      internal::CreateFbsVectorString(builder, current_cpp_compile_flags_);
  auto fbs_link_flags =
      internal::CreateFbsVectorString(builder, current_link_flags_);

  auto fbs_compile_dependencies =
      internal::CreateFbsVectorPath(builder, current_compile_dependencies_);
  auto fbs_link_dependencies =
      internal::CreateFbsVectorPath(builder, current_link_dependencies_);

  auto fbs_target = fbs::CreateTargetDirect(
      builder, name_.c_str(), fbs_target_type, &fbs_source_files,
      &fbs_header_files, &fbs_lib_deps, &fbs_external_lib_deps,
      &fbs_include_dirs, &fbs_lib_dirs, &fbs_preprocessor_flags,
      &fbs_common_compile_flags, &fbs_c_compile_flags, &fbs_cpp_compile_flags,
      &fbs_link_flags, &fbs_compile_dependencies, &fbs_link_dependencies);
  fbs::FinishTargetBuffer(builder, fbs_target);

  auto file_path = GetBinaryPath();
  return env::SaveFile(file_path.string().c_str(),
                       (const char *)builder.GetBufferPointer(),
                       builder.GetSize(), true);
}

} // namespace buildcc::base
