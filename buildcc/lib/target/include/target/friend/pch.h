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

#ifndef TARGET_FRIEND_PCH_H_
#define TARGET_FRIEND_PCH_H_

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace buildcc::base {

class Target;

class Pch {
public:
  Pch(Target &target)
      : target_(target), header_path_(ConstructHeaderPath()),
        compile_path_(ConstructCompilePath()) {}

  // NOTE, These APIs should be called inside `Target::Build`
  void CacheCompileCommand();
  void PchTask();

  const fs::path &GetHeaderPath() const { return header_path_; }
  const fs::path &GetCompilePath() const { return compile_path_; }

private:
  // Each target only has only 1 PCH file
  fs::path ConstructHeaderPath() const;
  fs::path ConstructCompilePath() const;

  std::string ConstructCompileCommand() const;

  void PreCompile();
  void BuildCompile();

private:
  Target &target_;

  fs::path header_path_;
  fs::path compile_path_;
  std::string command_;
};

} // namespace buildcc::base

#endif
