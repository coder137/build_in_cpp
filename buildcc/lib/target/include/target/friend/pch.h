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

#include <string>

namespace buildcc::base {

class Target;

class Pch {
public:
  Pch(Target &target) : target_(target) {}

  std::string ConstructPchCompileCommand() const;

  void PrePchCompile();
  void BuildPchCompile();

  void PchTask();

private:
  Target &target_;
};

} // namespace buildcc::base

#endif
