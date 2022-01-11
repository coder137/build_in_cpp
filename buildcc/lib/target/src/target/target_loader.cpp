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

#include "target/base/target_loader.h"

#include "env/logging.h"
#include "env/util.h"

// Private
#include "target/private/schema_util.h"
#include "target_generated.h"

namespace buildcc::internal {

// Public functions
bool TargetLoader::Load() {
  env::log_trace(name_, __FUNCTION__);

  auto file_path = GetBinaryPath();
  std::string buffer;
  bool is_loaded =
      env::load_file(path_as_string(file_path).c_str(), true, &buffer);
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

  extract_path(target->source_files(), loaded_sources_);
  extract_path(target->header_files(), loaded_headers_);
  extract_path(target->pch_files(), loaded_pchs_);
  extract_path(target->lib_deps(), loaded_lib_deps_);

  extract(target->external_lib_deps(), loaded_external_lib_dirs_);

  extract(target->include_dirs(), loaded_include_dirs_);
  extract(target->lib_dirs(), loaded_lib_dirs_);

  extract(target->preprocessor_flags(), loaded_preprocessor_flags_);
  extract(target->common_compile_flags(), loaded_common_compile_flags_);
  extract(target->pch_compile_flags(), loaded_pch_compile_flags_);
  extract(target->pch_object_flags(), loaded_pch_object_flags_);
  extract(target->asm_compile_flags(), loaded_asm_compile_flags_);
  extract(target->c_compile_flags(), loaded_c_compile_flags_);
  extract(target->cpp_compile_flags(), loaded_cpp_compile_flags_);
  extract(target->link_flags(), loaded_link_flags_);

  extract_path(target->compile_dependencies(), loaded_compile_dependencies_);
  extract_path(target->link_dependencies(), loaded_link_dependencies_);

  loaded_pch_compiled_ = target->pch_compiled();
  loaded_target_linked_ = target->target_linked();

  loaded_ = true;
  return true;
}

// Private functions
void TargetLoader::Initialize() {}

} // namespace buildcc::internal
