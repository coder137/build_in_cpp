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

#ifndef TARGET_API_COPY_API_H_
#define TARGET_API_COPY_API_H_

#include <initializer_list>

namespace buildcc::base {

enum class CopyOption {
  SourceFiles,
  HeaderFiles,
  PchFiles,
  LibDeps,
  IncludeDirs,
  LibDirs,
  ExternalLibDeps,
  PreprocessorFlags,
  CommonCompileFlags,
  PchCompileFlags,
  PchObjectFlags,
  AsmCompileFlags,
  CCompileFlags,
  CppCompileFlags,
  LinkFlags,
  CompileDependencies,
  LinkDependencies,
};

// Requires
// - TargetStorer
// - TargetState
template <typename T> class CopyApi {
public:
  /**
   * @brief Copy when Target supplied by const reference
   */
  void Copy(const T &target, std::initializer_list<CopyOption> options);

  /**
   * @brief Copy when Target supplied by move
   */
  void Copy(T &&target, std::initializer_list<CopyOption> options);

private:
  template <typename TargetType>
  void SpecializedCopy(TargetType target,
                       std::initializer_list<CopyOption> options);
};

} // namespace buildcc::base

namespace buildcc {

typedef base::CopyOption CopyOption;

}

#endif
