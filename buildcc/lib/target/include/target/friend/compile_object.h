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

#ifndef TARGET_FRIEND_COMPILE_OBJECT_H_
#define TARGET_FRIEND_COMPILE_OBJECT_H_

#include <filesystem>

#include "target/common/path.h"

#include "taskflow/core/task.hpp"
#include "taskflow/taskflow.hpp"

namespace fs = std::filesystem;

namespace buildcc {

class Target;

}

namespace buildcc::base {

class CompileObject {

public:
  struct ObjectData {
    ObjectData(const fs::path &o, const std::string &c)
        : output(o), command(c) {}

    fs::path output;
    std::string command;
  };

public:
  CompileObject(Target &target) : target_(target) {}

  void AddObjectData(const fs::path &absolute_source_path);

  void CacheCompileCommands();
  void Task();

  const ObjectData &GetObjectData(const fs::path &absolute_source) const;
  const std::unordered_map<fs::path, ObjectData, internal::PathHash> &
  GetObjectDataMap() const {
    return object_files_;
  }
  fs_unordered_set GetCompiledSources() const;
  tf::Task &GetTask() { return compile_task_; }

private:
  fs::path ConstructObjectPath(const fs::path &absolute_source_file) const;

  void BuildObjectCompile(std::vector<internal::Path> &source_files,
                          std::vector<internal::Path> &dummy_source_files);

  void PreObjectCompile();

  void CompileSources(std::vector<internal::Path> &source_files);
  void RecompileSources(std::vector<internal::Path> &source_files,
                        std::vector<internal::Path> &dummy_source_files);

private:
  Target &target_;

  std::unordered_map<fs::path, ObjectData, internal::PathHash> object_files_;
  tf::Task compile_task_;
};

} // namespace buildcc::base

#endif
