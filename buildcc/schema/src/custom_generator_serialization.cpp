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
  // Verified, does not need a nullptr check

  const auto *fbs_rels = custom_generator->rels();
  if (fbs_rels == nullptr) {
    return false;
  }

  // rel_io->id is a required parameter, hence rel_io cannot be nullptr
  for (const auto *rel_io : *fbs_rels) {
    RelInputOutputSchema schema;
    extract_path(rel_io->inputs(), schema.internal_inputs);
    extract(rel_io->outputs(), schema.outputs);
    load_.internal_rels_map.emplace(rel_io->id()->c_str(), std::move(schema));
  }
  return true;
}

bool CustomGeneratorSerialization::Store(
    const fs::path &absolute_serialized_file) {
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<fbs::RelInputOutput>> fbs_rels;
  for (const auto &rel_miter : store_.internal_rels_map) {
    const auto &id = rel_miter.first;
    const auto &rel_io = rel_miter.second;
    auto fbs_internal_inputs =
        internal::create_fbs_vector_path(builder, rel_io.internal_inputs);
    auto fbs_outputs =
        internal::create_fbs_vector_string(builder, rel_io.outputs);
    auto fbs_rel = fbs::CreateRelInputOutputDirect(
        builder, id.c_str(), &fbs_internal_inputs, &fbs_outputs);
    fbs_rels.push_back(fbs_rel);
  }

  auto fbs_generator =
      fbs::CreateCustomGeneratorDirect(builder, store_.name.c_str(), &fbs_rels);
  fbs::FinishCustomGeneratorBuffer(builder, fbs_generator);

  return env::save_file(path_as_string(absolute_serialized_file).c_str(),
                        (const char *)builder.GetBufferPointer(),
                        builder.GetSize(), true);
}

} // namespace buildcc::internal
