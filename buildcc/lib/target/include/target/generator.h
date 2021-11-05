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

#ifndef TARGET_GENERATOR_H_
#define TARGET_GENERATOR_H_

#include <functional>
#include <string>
#include <vector>

#include "taskflow/taskflow.hpp"

#include "env/env.h"

#include "target/builder_interface.h"

#include "target/generator_loader.h"
#include "target/path.h"

namespace buildcc::base {

// TODO, Add root and build path
class Generator : public BuilderInterface {
public:
  Generator(const std::string &name,
            const fs::path &target_path_relative_to_root,
            const internal::fs_unordered_set &inputs,
            const internal::fs_unordered_set &outputs,
            const std::vector<std::string> &commands, bool parallel = false)
      : name_(name), loader_(name, target_path_relative_to_root),
        current_input_files_({}, inputs), current_output_files_(outputs),
        current_commands_(commands), parallel_(parallel) {
    unique_id_ = name;
  }
  Generator(const Generator &generator) = delete;

  // TODO, Add Global arguments

  // TODO, Add your own custom command

  void Build() override;

  // Getter
  fs::path GetBinaryPath() const { return loader_.GetBinaryPath(); }
  tf::Taskflow &GetTaskflow() { return tf_; }

private:
  void GenerateTask(tf::FlowBuilder &builder);
  void Convert();
  void BuildGenerate();

  bool Store() override;

  // Recheck states
  void InputRemoved();
  void InputAdded();
  void InputUpdated();

  void OutputChanged();
  void CommandChanged();

private:
  // Constructor
  std::string name_;
  internal::GeneratorLoader loader_;

  // Serialization
  internal::default_files current_input_files_;
  internal::fs_unordered_set current_output_files_;
  std::vector<std::string> current_commands_;
  bool parallel_{false};

  tf::Taskflow tf_;
};

} // namespace buildcc::base

#endif
