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

#ifndef TARGET_FRIEND_LINK_TARGET_H_
#define TARGET_FRIEND_LINK_TARGET_H_

#include <filesystem>
#include <string>

#include "taskflow/taskflow.hpp"

namespace fs = std::filesystem;

namespace buildcc {

class Target;

}

namespace buildcc::internal {

class LinkTarget {
public:
  LinkTarget(Target &target)
      : target_(target), output_(ConstructOutputPath()) {}

  void CacheLinkCommand();
  void Task();

  const fs::path &GetOutput() const { return output_; }
  const std::string &GetCommand() const { return command_; }
  tf::Task &GetTask() { return task_; }

private:
  void BuildLink();
  void PreLink();

  fs::path ConstructOutputPath() const;

private:
  Target &target_;

  fs::path output_;
  std::string command_;
  tf::Task task_;
};

} // namespace buildcc::internal

#endif
