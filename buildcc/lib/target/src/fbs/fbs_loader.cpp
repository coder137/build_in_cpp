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

#include "target/fbs_loader.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

#include "env/logging.h"

namespace fbs = schema::internal;

namespace {
void ExtractPath(
    const flatbuffers::Vector<flatbuffers::Offset<schema::internal::Path>>
        *fbs_paths,
    buildcc::internal::path_unordered_set &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(buildcc::internal::Path::CreateNewPath(
        iter->pathname()->c_str(), iter->last_write_timestamp()));
  }
}

template <typename T>
void Extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
                 *fbs_paths,
             T &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(iter->str());
  }
}

} // namespace

namespace buildcc::internal {

// Public functions
bool FbsLoader::Load() {
  env::log_trace(name_, __FUNCTION__);

  auto file_path = GetBinaryPath();
  std::string buffer;
  bool is_loaded =
      flatbuffers::LoadFile(file_path.string().c_str(), true, &buffer);
  if (!is_loaded) {
    return false;
  }

  flatbuffers::Verifier verifier((const uint8_t *)buffer.c_str(),
                                 buffer.length());
  const bool is_verified = fbs::VerifyTargetBuffer(verifier);
  if (!is_verified) {
    return false;
  }

  const auto *target = fbs::GetTarget((const void *)buffer.c_str());
  // target->name()->c_str();
  // target->type();

  ExtractPath(target->source_files(), loaded_sources_);
  ExtractPath(target->header_files(), loaded_headers_);
  ExtractPath(target->lib_deps(), loaded_lib_deps_);

  Extract(target->external_lib_deps(), loaded_external_lib_dirs_);

  Extract(target->include_dirs(), loaded_include_dirs_);
  Extract(target->lib_dirs(), loaded_lib_dirs_);

  Extract(target->preprocessor_flags(), loaded_preprocessor_flags_);
  Extract(target->c_compile_flags(), loaded_c_compile_flags_);
  Extract(target->cpp_compile_flags(), loaded_cpp_compile_flags_);
  Extract(target->link_flags(), loaded_link_flags_);

  loaded_ = true;
  return true;
}

// Private functions
void FbsLoader::Initialize() {}

} // namespace buildcc::internal
