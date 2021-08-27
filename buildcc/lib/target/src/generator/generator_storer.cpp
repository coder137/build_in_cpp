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

  std::vector<flatbuffers::Offset<fbs::GenInfo>> fbs_generator_list;
  for (const auto &info : current_info_) {
    const auto &geninfo = info.second;
    auto fbs_inputs = internal::CreateFbsVectorPath(builder, geninfo.inputs);
    auto fbs_outputs =
        internal::CreateFbsVectorString(builder, geninfo.outputs);
    auto fbs_commands =
        internal::CreateFbsVectorString(builder, geninfo.commands);
    auto fbs_geninfo =
        fbs::CreateGenInfoDirect(builder, geninfo.name.c_str(), &fbs_inputs,
                                 &fbs_outputs, &fbs_commands, geninfo.parallel);
    fbs_generator_list.push_back(fbs_geninfo);
  }

  auto fbs_generator =
      fbs::CreateGeneratorDirect(builder, name_.c_str(), &fbs_generator_list);
  fbs::FinishGeneratorBuffer(builder, fbs_generator);

  auto file_path = GetBinaryPath();
  return env::SaveFile(file_path.string().c_str(),
                       (const char *)builder.GetBufferPointer(),
                       builder.GetSize(), true);
}

} // namespace buildcc::base
