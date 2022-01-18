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

#ifndef TARGET_FRIEND_COMPILE_PCH_H_
#define TARGET_FRIEND_COMPILE_PCH_H_

#include <filesystem>
#include <string>

#include "taskflow/taskflow.hpp"

namespace fs = std::filesystem;

namespace buildcc {

class Target;

}

namespace buildcc::internal {

class CompilePch {
public:
  CompilePch(Target &target)
      : target_(target), header_path_(ConstructHeaderPath()),
        compile_path_(ConstructCompilePath()),
        object_path_(ConstructObjectPath()) {}

  // NOTE, These APIs should be called inside `Target::Build`
  void CacheCompileCommand();
  void Task();

  const fs::path &GetHeaderPath() const { return header_path_; }
  const fs::path &GetCompilePath() const { return compile_path_; }
  const fs::path &GetObjectPath() const { return object_path_; }

  // Call after BUILD
  const fs::path &GetSourcePath() const { return source_path_; }
  tf::Task &GetTask() { return task_; }

private:
  // Each target only has only 1 PCH file
  fs::path ConstructHeaderPath() const;
  fs::path ConstructCompilePath() const;
  fs::path ConstructObjectPath() const;

  // Needs to checks for C source extension vs Cpp source extension
  fs::path ConstructSourcePath(bool has_cpp) const;

  std::string ConstructCompileCommand() const;

  void PreCompile();
  void BuildCompile();

private:
  Target &target_;

  fs::path header_path_;
  fs::path compile_path_;

  // NOTE, Certain compilers (MSVC) require an input source with the header file
  // The corresponding object_path has also been added here for usage
  // `source_path_` is added as KEY: input_source locally for pch_command_
  // `object_path` is added as KEY: pch_objet_output globally (mainly to be used
  // during linking phase)
  fs::path source_path_;
  fs::path object_path_;

  std::string command_;

  tf::Task task_;
};

} // namespace buildcc::internal

#endif
