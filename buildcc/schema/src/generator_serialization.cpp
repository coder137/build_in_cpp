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

#include "schema/generator_serialization.h"

// Third party
#include "flatbuffers/flatbuffers.h"

// Private
#include "schema/schema_util.h"

// Schema generated
#include "generator_generated.h"

namespace buildcc::internal {

// PRIVATE

bool GeneratorSerialization::Verify(const std::string &serialized_data) {
  flatbuffers::Verifier verifier((const uint8_t *)serialized_data.c_str(),
                                 serialized_data.length());
  return fbs::VerifyGeneratorBuffer(verifier);
}

bool GeneratorSerialization::Load(const std::string &serialized_data) {
  const auto *generator =
      fbs::GetGenerator((const void *)serialized_data.c_str());
  if (generator == nullptr) {
    return false;
  }

  extract_path(generator->inputs(), load_.internal_inputs);
  extract(generator->outputs(), load_.outputs);
  extract(generator->commands(), load_.commands);
  return true;
}

bool GeneratorSerialization::Store(const fs::path &absolute_serialized_file) {
  flatbuffers::FlatBufferBuilder builder;

  auto fbs_input_files =
      internal::create_fbs_vector_path(builder, store_.internal_inputs);
  auto fbs_output_files =
      internal::create_fbs_vector_string(builder, store_.outputs);
  auto fbs_commands =
      internal::create_fbs_vector_string(builder, store_.commands);

  auto fbs_generator =
      fbs::CreateGeneratorDirect(builder, store_.name.c_str(), &fbs_input_files,
                                 &fbs_output_files, &fbs_commands);
  fbs::FinishGeneratorBuffer(builder, fbs_generator);

  return env::save_file(path_as_string(absolute_serialized_file).c_str(),
                        (const char *)builder.GetBufferPointer(),
                        builder.GetSize(), true);
}

} // namespace buildcc::internal
