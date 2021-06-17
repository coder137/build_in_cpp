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

#include "clang_compile_commands.h"

#include <algorithm>

// env
#include "assert_fatal.h"

// target
#include "internal/util.h"

// flatbuffers
#include "flatbuffers/flexbuffers.h"

namespace buildcc::plugin {

void ClangCompileCommands::AddTarget(const base::Target *target) {
  env::assert_fatal(target != nullptr, "Target should not be NULL");
  targets_.push_back(target);
}

void ClangCompileCommands::Generate() {
  // Early terminate if rebuild is not required
  const bool regenerate = std::any_of(
      targets_.begin(), targets_.end(), [](const base::Target *target) {
        return (target->FirstBuild() || target->Rebuild());
      });
  if (!regenerate) {
    env::log_trace("ClangCompileCommands", "Generate -> false");
    return;
  }
  env::log_trace("ClangCompileCommands", "Generate -> true");

  // DONE, Create custom flexbuffer binary compile_commands interface
  flexbuffers::Builder fbb;
  size_t start = fbb.StartVector();

  for (const auto *t : targets_) {
    const auto &source_files = t->GetCurrentSourceFiles();

    for (const auto &f : source_files) {
      // DONE, Get source list name
      // DONE, Get std::vector<std::string> CompileCommand
      // DONE, Get intermediate directory from env
      const auto input_file = f.GetPathname();
      const auto command = t->CompileCommand(input_file);
      const auto directory = env::get_project_build_dir();

      // DONE, Use flatbuffers::Flexbuffer to create binary format
      fbb.Map([&]() {
        fbb.String("directory", directory.string());
        fbb.String("command", command);
        fbb.String("file", input_file.string());
      });
    }
  }

  fbb.EndVector(start, false, false);
  fbb.Finish();

  // DONE, Convert to json
  std::string compile_commands;
  flexbuffers::GetRoot(fbb.GetBuffer()).ToString(true, true, compile_commands);

  // DONE, Save file using the flatbuffers::SaveFile utility function
  std::filesystem::path file =
      std::filesystem::path(buildcc::env::get_project_build_dir()) /
      "compile_commands.json";
  bool saved =
      flatbuffers::SaveFile(file.string().c_str(), compile_commands, false);
  env::assert_fatal(saved, "Could not save compile_commands.json");
}

} // namespace buildcc::plugin
