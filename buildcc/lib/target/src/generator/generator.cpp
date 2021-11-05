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

#include <algorithm>

#include "env/assert_fatal.h"

namespace buildcc::base {

void Generator::Build() {
  (void)loader_.Load();
  GenerateTask();
}

// PRIVATE

void Generator::Convert() { current_input_files_.Convert(); }

void Generator::BuildGenerate() {
  if (!loader_.IsLoaded()) {
    dirty_ = true;
  } else {
    RecheckPaths(
        loader_.GetLoadedInputFiles(), current_input_files_.internal,
        [&]() { InputRemoved(); }, [&]() { InputAdded(); },
        [&]() { InputUpdated(); });
    RecheckChanged(loader_.GetLoadedOutputFiles(), current_output_files_,
                   [&]() { OutputChanged(); });
    RecheckChanged(loader_.GetLoadedCommands(), current_commands_,
                   [&]() { CommandChanged(); });
  }
}

} // namespace buildcc::base
