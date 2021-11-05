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

#ifndef TARGET_GENERATOR_LOADER_H_
#define TARGET_GENERATOR_LOADER_H_

#include "target/loader_interface.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "fmt/format.h"

#include "target/path.h"

namespace buildcc::internal {

class GeneratorLoader : public LoaderInterface {
public:
  GeneratorLoader(const std::string &name, const fs::path &absolute_path)
      : name_(name), path_(absolute_path) {}

  GeneratorLoader(const GeneratorLoader &loader) = delete;

  bool Load() override;

  // Getters
  fs::path GetBinaryPath() const override {
    return path_ / fmt::format("{}.bin", name_);
  }

  const internal::path_unordered_set &GetLoadedInputFiles() const {
    return loaded_input_files_;
  }

  const internal::fs_unordered_set &GetLoadedOutputFiles() const {
    return loaded_output_files_;
  }

  const std::vector<std::string> &GetLoadedCommands() const {
    return loaded_commands_;
  }

private:
  std::string name_;
  fs::path path_;

  internal::path_unordered_set loaded_input_files_;
  internal::fs_unordered_set loaded_output_files_;
  std::vector<std::string> loaded_commands_;
};

} // namespace buildcc::internal

#endif
