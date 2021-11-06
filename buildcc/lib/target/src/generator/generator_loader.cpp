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

#include "target/generator_loader.h"

#include "env/logging.h"
#include "env/util.h"

// Private
#include "target/private/schema_util.h"

#include "generator_generated.h"

namespace buildcc::internal {

bool GeneratorLoader::Load() {
  env::log_trace(name_, __FUNCTION__);

  auto file_path = GetBinaryPath();
  std::string buffer;
  bool is_loaded = env::LoadFile(file_path.string().c_str(), true, &buffer);
  if (!is_loaded) {
    return false;
  }

  flatbuffers::Verifier verifier((const uint8_t *)buffer.c_str(),
                                 buffer.length());
  const bool is_verified = fbs::VerifyGeneratorBuffer(verifier);
  if (!is_verified) {
    return false;
  }

  const auto *generator = fbs::GetGenerator((const void *)buffer.c_str());

  ExtractPath(generator->inputs(), loaded_input_files_);
  Extract(generator->outputs(), loaded_output_files_);
  Extract(generator->commands(), loaded_commands_);

  loaded_ = true;
  return true;
}

} // namespace buildcc::internal
