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

#ifndef TARGET_CUSTOM_GENERATOR_CUSTOM_GENERATOR_CONTEXT_H_
#define TARGET_CUSTOM_GENERATOR_CUSTOM_GENERATOR_CONTEXT_H_

#include "schema/path.h"

#include "env/command.h"

namespace buildcc {

class CustomGeneratorContext {
public:
  CustomGeneratorContext(const env::Command &c, const fs_unordered_set &i,
                         const fs_unordered_set &o,
                         const std::vector<uint8_t> &ub)
      : command(c), inputs(i), outputs(o), userblob(ub) {}

  const env::Command &command;
  const fs_unordered_set &inputs;
  const fs_unordered_set &outputs;
  const std::vector<uint8_t> &userblob;
};

// clang-format off
using GenerateCb = std::function<bool (const CustomGeneratorContext &)>;
// clang-format on

} // namespace buildcc

#endif
