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

#include "schema/custom_generator_serialization.h"

// Third party
#include "flatbuffers/flatbuffers.h"

// Private
#include "schema/private/schema_util.h"

// Schema generated
#include "custom_generator_generated.h"

namespace buildcc::internal {

// PRIVATE

bool CustomGeneratorSerialization::Verify(const std::string &serialized_data) {
  flatbuffers::Verifier verifier((const uint8_t *)serialized_data.c_str(),
                                 serialized_data.length());
  return fbs::VerifyCustomGeneratorBuffer(verifier);
}

bool CustomGeneratorSerialization::Load(const std::string &serialized_data) {
  const auto *custom_generator =
      fbs::GetCustomGenerator((const void *)serialized_data.c_str());
  if (custom_generator == nullptr) {
    return false;
  }

  extract_path(custom_generator->recheck_path_dep(),
               load_.internal_recheck_path_dep);
  extract(custom_generator->recheck_string_dep(), load_.recheck_string_dep);
  return true;
}

bool CustomGeneratorSerialization::Store(
    const fs::path &absolute_serialized_file) {
  flatbuffers::FlatBufferBuilder builder;

  auto fbs_recheck_path_dep = internal::create_fbs_vector_path(
      builder, store_.internal_recheck_path_dep);
  auto fbs_recheck_string_dep =
      internal::create_fbs_vector_string(builder, store_.recheck_string_dep);

  auto fbs_generator = fbs::CreateCustomGeneratorDirect(
      builder, store_.name.c_str(), &fbs_recheck_path_dep,
      &fbs_recheck_string_dep);
  fbs::FinishCustomGeneratorBuffer(builder, fbs_generator);

  return env::save_file(path_as_string(absolute_serialized_file).c_str(),
                        (const char *)builder.GetBufferPointer(),
                        builder.GetSize(), true);
}

} // namespace buildcc::internal
