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

#include "schema/target_serialization.h"

// Third party
#include "flatbuffers/flatbuffers.h"

// Private
#include "schema/schema_util.h"

// Schema generated
#include "target_generated.h"

namespace buildcc::internal {

// PUBLIC
void TargetSerialization::UpdatePchCompiled(const TargetSchema &store) {
  store_.internal_pchs = store.internal_pchs;
  store_.pch_compiled = true;
}
void TargetSerialization::AddSource(const internal::Path &source) {
  std::lock_guard<std::mutex> guard(add_source_mutex);
  store_.internal_sources.insert(source);
}

void TargetSerialization::UpdateTargetCompiled() {
  store_.target_linked = true;
}

void TargetSerialization::UpdateStore(const TargetSchema &store) {
  TargetSchema temp = store;
  temp.internal_pchs = store_.internal_pchs;
  temp.pch_compiled = store_.pch_compiled;
  temp.internal_sources = store_.internal_sources;
  temp.target_linked = store_.target_linked;
  store_ = std::move(temp);
}

// PRIVATE
bool TargetSerialization::Verify(const std::string &serialized_data) {
  flatbuffers::Verifier verifier((const uint8_t *)serialized_data.c_str(),
                                 serialized_data.length());
  return fbs::VerifyTargetBuffer(verifier);
}

bool TargetSerialization::Load(const std::string &serialized_data) {
  const auto *target = fbs::GetTarget((const void *)serialized_data.c_str());
  if (target == nullptr) {
    return false;
  }

  extract_path(target->source_files(), load_.internal_sources);
  extract_path(target->header_files(), load_.internal_headers);
  extract_path(target->pch_files(), load_.internal_pchs);
  extract_path(target->lib_deps(), load_.internal_libs);

  extract(target->external_lib_deps(), load_.external_libs);

  extract(target->include_dirs(), load_.include_dirs);
  extract(target->lib_dirs(), load_.lib_dirs);

  extract(target->preprocessor_flags(), load_.preprocessor_flags);
  extract(target->common_compile_flags(), load_.common_compile_flags);
  extract(target->pch_compile_flags(), load_.pch_compile_flags);
  extract(target->pch_object_flags(), load_.pch_object_flags);
  extract(target->asm_compile_flags(), load_.asm_compile_flags);
  extract(target->c_compile_flags(), load_.c_compile_flags);
  extract(target->cpp_compile_flags(), load_.cpp_compile_flags);
  extract(target->link_flags(), load_.link_flags);

  extract_path(target->compile_dependencies(),
               load_.internal_compile_dependencies);
  extract_path(target->link_dependencies(), load_.internal_link_dependencies);

  load_.pch_compiled = target->pch_compiled();
  load_.target_linked = target->target_linked();
  return true;
}

bool TargetSerialization::Store(const fs::path &absolute_serialized_file) {
  flatbuffers::FlatBufferBuilder builder;

  auto fbs_target_type = (fbs::TargetType)store_.type;

  auto fbs_source_files =
      internal::create_fbs_vector_path(builder, store_.internal_sources);
  auto fbs_header_files =
      internal::create_fbs_vector_path(builder, store_.internal_headers);
  auto fbs_pch_files =
      internal::create_fbs_vector_path(builder, store_.internal_pchs);

  auto fbs_lib_deps =
      internal::create_fbs_vector_path(builder, store_.internal_libs);
  auto fbs_external_lib_deps =
      internal::create_fbs_vector_string(builder, store_.external_libs);

  auto fbs_include_dirs =
      internal::create_fbs_vector_string(builder, store_.include_dirs);
  auto fbs_lib_dirs =
      internal::create_fbs_vector_string(builder, store_.lib_dirs);

  auto fbs_preprocessor_flags =
      internal::create_fbs_vector_string(builder, store_.preprocessor_flags);
  auto fbs_common_compile_flags =
      internal::create_fbs_vector_string(builder, store_.common_compile_flags);
  auto fbs_pch_compile_flags =
      internal::create_fbs_vector_string(builder, store_.pch_compile_flags);
  auto fbs_pch_object_flags =
      internal::create_fbs_vector_string(builder, store_.pch_object_flags);
  auto fbs_asm_compile_flags =
      internal::create_fbs_vector_string(builder, store_.asm_compile_flags);
  auto fbs_c_compile_flags =
      internal::create_fbs_vector_string(builder, store_.c_compile_flags);
  auto fbs_cpp_compile_flags =
      internal::create_fbs_vector_string(builder, store_.cpp_compile_flags);
  auto fbs_link_flags =
      internal::create_fbs_vector_string(builder, store_.link_flags);

  auto fbs_compile_dependencies = internal::create_fbs_vector_path(
      builder, store_.internal_compile_dependencies);
  auto fbs_link_dependencies = internal::create_fbs_vector_path(
      builder, store_.internal_link_dependencies);

  auto fbs_target = fbs::CreateTargetDirect(
      builder, store_.name.c_str(), fbs_target_type, &fbs_source_files,
      &fbs_header_files, &fbs_pch_files, &fbs_lib_deps, &fbs_external_lib_deps,
      &fbs_include_dirs, &fbs_lib_dirs, &fbs_preprocessor_flags,
      &fbs_common_compile_flags, &fbs_pch_compile_flags, &fbs_pch_object_flags,
      &fbs_asm_compile_flags, &fbs_c_compile_flags, &fbs_cpp_compile_flags,
      &fbs_link_flags, &fbs_compile_dependencies, &fbs_link_dependencies,
      store_.pch_compiled, store_.target_linked);
  fbs::FinishTargetBuffer(builder, fbs_target);

  return env::save_file(path_as_string(absolute_serialized_file).c_str(),
                        (const char *)builder.GetBufferPointer(),
                        builder.GetSize(), true);
}

} // namespace buildcc::internal
