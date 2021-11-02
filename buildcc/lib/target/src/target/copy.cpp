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

#include "env/assert_fatal.h"
#include "target/target.h"
#include <algorithm>

namespace {

template <typename T, typename C> void CopyVar(const T &source, const C &cb) {
  for (const auto &s : source) {
    cb(s);
  }
}

} // namespace

namespace buildcc::base {

// PUBLIC

// NOTE, std::move performs a copy when `const Target &`
void Target::Copy(const Target &target,
                  std::initializer_list<CopyOption> options) {
  LockedAfterBuild();
  for (const CopyOption o : options) {
    switch (o) {
    case CopyOption::PreprocessorFlags:
      CopyVar(target.storer_.current_preprocessor_flags,
              [&](const auto &f) { AddPreprocessorFlag(f); });
      break;
    case CopyOption::CommonCompileFlags:
      CopyVar(target.storer_.current_common_compile_flags,
              [&](const auto &f) { AddCommonCompileFlag(f); });
      break;
    case CopyOption::PchCompileFlags:
      CopyVar(target.storer_.current_pch_compile_flags,
              [&](const auto &f) { AddPchCompileFlag(f); });
      break;
    case CopyOption::PchObjectFlags:
      CopyVar(target.storer_.current_pch_object_flags,
              [&](const auto &f) { AddPchObjectFlag(f); });
      break;
    case CopyOption::AsmCompileFlags:
      CopyVar(target.storer_.current_asm_compile_flags,
              [&](const auto &f) { AddAsmCompileFlag(f); });
      break;
    case CopyOption::CCompileFlags:
      CopyVar(target.storer_.current_c_compile_flags,
              [&](const auto &f) { AddCCompileFlag(f); });
      break;
    case CopyOption::CppCompileFlags:
      CopyVar(target.storer_.current_cpp_compile_flags,
              [&](const auto &f) { AddCppCompileFlag(f); });
      break;
    case CopyOption::LinkFlags:
      CopyVar(target.storer_.current_link_flags,
              [&](const auto &f) { AddLinkFlag(f); });
      break;
    case CopyOption::CompileDependencies:
      CopyVar(target.storer_.current_compile_dependencies.user,
              [&](const auto &f) { AddCompileDependency(f); });
      break;
    case CopyOption::LinkDependencies:
      CopyVar(target.storer_.current_link_dependencies.user,
              [&](const auto &f) { AddLinkDependency(f); });
      break;
    case CopyOption::SourceFiles:
      CopyVar(target.compile_object_.GetObjectDataMap(), [&](const auto &m) {
        AddSourceAbsolute(m.first, m.second.output);
      });
      break;
    case CopyOption::HeaderFiles:
      CopyVar(target.storer_.current_header_files.user,
              [&](const auto &f) { AddHeaderAbsolute(f); });
      break;
    case CopyOption::PchFiles:
      CopyVar(target.storer_.current_pch_files.user,
              [&](const auto &f) { AddPchAbsolute(f); });
      break;
    case CopyOption::LibDeps:
      CopyVar(target.storer_.current_lib_deps.user,
              [&](const auto &f) { AddLibDep(f.string()); });
      break;
    case CopyOption::IncludeDirs:
      CopyVar(target.storer_.current_include_dirs,
              [&](const auto &dir) { AddIncludeDirAbsolute(dir); });
      break;
    case CopyOption::LibDirs:
      CopyVar(target.storer_.current_lib_dirs,
              [&](const auto &dir) { AddLibDir(dir); });
      break;
    case CopyOption::ExternalLibDeps:
      CopyVar(target.storer_.current_external_lib_deps,
              [&](const auto &f) { AddLibDep(f); });
      break;
    default:
      env::assert_fatal<false>("Invalid Option added");
      break;
    }
  }
}

} // namespace buildcc::base
