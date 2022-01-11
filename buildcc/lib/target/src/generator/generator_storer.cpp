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

#include "target/generator.h"

#include "flatbuffers/flatbuffers.h"

#include "env/util.h"

#include "generator_generated.h"
#include "target/private/schema_util.h"

namespace fbs = schema::internal;

namespace buildcc::base {

bool Generator::Store() {
  env::log_trace(name_, __FUNCTION__);

  flatbuffers::FlatBufferBuilder builder;

  auto fbs_input_files =
      internal::create_fbs_vector_path(builder, current_input_files_.internal);
  auto fbs_output_files =
      internal::create_fbs_vector_string(builder, current_output_files_);
  auto fbs_commands =
      internal::create_fbs_vector_string(builder, current_commands_);

  auto fbs_generator =
      fbs::CreateGeneratorDirect(builder, name_.c_str(), &fbs_input_files,
                                 &fbs_output_files, &fbs_commands);
  fbs::FinishGeneratorBuffer(builder, fbs_generator);

  const fs::path file_path = GetBinaryPath();
  return env::save_file(path_as_string(file_path).c_str(),
                        (const char *)builder.GetBufferPointer(),
                        builder.GetSize(), true);
}

} // namespace buildcc::base
