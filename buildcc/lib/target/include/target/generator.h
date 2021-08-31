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

class Generator : public BuilderInterface {
public:
  Generator(const std::string &name, const fs::path &path)
      : loader_(name, path) {}
  Generator(const Generator &generator) = delete;

  void AddGenInfo(const std::string &name,
                  const internal::fs_unordered_set &inputs,
                  const internal::fs_unordered_set &outputs,
                  const std::vector<std::string> &commands, bool parallel);
  void Build() override;

  // Getter
  fs::path GetBinaryPath() const { return loader_.GetBinaryPath(); }
  tf::Taskflow &GetTaskflow() { return tf_; }

private:
  void GenerateTask();
  // Convert from UserGenInfo to internal::GenInfo
  void Convert();
  std::vector<const internal::GenInfo *> BuildGenerate();
  bool Regenerate(std::vector<const internal::GenInfo *> &generated_files);

  bool Store() override;

  // Recheck states
  void InputRemoved();
  void InputAdded();
  void InputUpdated();

  void OutputChanged();
  void CommandChanged();

private:
  std::string name_;
  std::unordered_map<std::string, internal::GenInfo> current_info_;

  internal::GeneratorLoader loader_;

  tf::Taskflow tf_;
  tf::Task build_task_;
};

} // namespace buildcc::base

#endif
