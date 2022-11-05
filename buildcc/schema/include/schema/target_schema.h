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

#ifndef SCHEMA_TARGET_SCHEMA_H_
#define SCHEMA_TARGET_SCHEMA_H_

#include <string>
#include <vector>

#include "schema/path.h"
#include "schema/target_type.h"

namespace buildcc::internal {

struct TargetSchema {
  std::string name;
  TargetType type{TargetType::Undefined};

  path_unordered_set internal_sources;
  path_unordered_set internal_headers;
  path_unordered_set internal_pchs;
  std::vector<Path> internal_libs;
  std::vector<std::string> external_libs;

  fs_unordered_set include_dirs;
  fs_unordered_set lib_dirs;

  std::unordered_set<std::string> preprocessor_flags;
  std::unordered_set<std::string> common_compile_flags;
  std::unordered_set<std::string> pch_compile_flags;
  std::unordered_set<std::string> pch_object_flags;
  std::unordered_set<std::string> asm_compile_flags;
  std::unordered_set<std::string> c_compile_flags;
  std::unordered_set<std::string> cpp_compile_flags;
  std::unordered_set<std::string> link_flags;

  path_unordered_set internal_compile_dependencies;
  path_unordered_set internal_link_dependencies;

  bool pch_compiled{false};
  bool target_linked{false};

private:
  static constexpr const char *const kName = "name";
  static constexpr const char *const kType = "type";

  static constexpr const char *const kSources = "sources";
  static constexpr const char *const kHeaders = "headers";
  static constexpr const char *const kPchs = "pchs";
  static constexpr const char *const kLibs = "libs";
  static constexpr const char *const kExternalLibs = "external_libs";

  static constexpr const char *const kIncludeDirs = "include_dirs";
  static constexpr const char *const kLibDirs = "lib_dirs";

  static constexpr const char *const kPreprocessorFlags = "preprocessor_flags";
  static constexpr const char *const kCommonCompileFlags =
      "common_compile_flags";
  static constexpr const char *const kPchCompileFlags = "pch_compile_flags";
  static constexpr const char *const kPchObjectFlags = "pch_object_flags";
  static constexpr const char *const kAsmCompileFlags = "asm_compile_flags";
  static constexpr const char *const kCCompileFlags = "c_compile_flags";
  static constexpr const char *const kCppCompileFlags = "cpp_compile_flags";
  static constexpr const char *const kLinkFlags = "link_flags";

  static constexpr const char *const kCompileDependencies =
      "compile_dependencies";
  static constexpr const char *const kLinkDependencies = "link_dependencies";

  static constexpr const char *const kPchCompiled = "pch_compiled";
  static constexpr const char *const kTargetLinked = "target_linked";

public:
  friend void to_json(json &j, const TargetSchema &schema) {
    j[kName] = schema.name;
    j[kType] = schema.type;
    j[kSources] = schema.internal_sources;
    j[kHeaders] = schema.internal_headers;
    j[kPchs] = schema.internal_pchs;
    j[kLibs] = schema.internal_libs;
    j[kExternalLibs] = schema.external_libs;
    j[kIncludeDirs] = schema.include_dirs;
    j[kLibDirs] = schema.lib_dirs;

    j[kPreprocessorFlags] = schema.preprocessor_flags;
    j[kCommonCompileFlags] = schema.common_compile_flags;
    j[kPchCompileFlags] = schema.pch_compile_flags;
    j[kPchObjectFlags] = schema.pch_object_flags;
    j[kAsmCompileFlags] = schema.asm_compile_flags;
    j[kCCompileFlags] = schema.c_compile_flags;
    j[kCppCompileFlags] = schema.cpp_compile_flags;
    j[kLinkFlags] = schema.link_flags;

    j[kCompileDependencies] = schema.internal_compile_dependencies;
    j[kLinkDependencies] = schema.internal_link_dependencies;
    j[kPchCompiled] = schema.pch_compiled;
    j[kTargetLinked] = schema.target_linked;
  }

  friend void from_json(const json &j, TargetSchema &schema) {
    j.at(kName).get_to(schema.name);
    j.at(kType).get_to(schema.type);
    j.at(kSources).get_to(schema.internal_sources);
    j.at(kHeaders).get_to(schema.internal_headers);
    j.at(kPchs).get_to(schema.internal_pchs);
    j.at(kLibs).get_to(schema.internal_libs);
    j.at(kExternalLibs).get_to(schema.external_libs);
    j.at(kIncludeDirs).get_to(schema.include_dirs);
    j.at(kLibDirs).get_to(schema.lib_dirs);

    j.at(kPreprocessorFlags).get_to(schema.preprocessor_flags);
    j.at(kCommonCompileFlags).get_to(schema.common_compile_flags);
    j.at(kPchCompileFlags).get_to(schema.pch_compile_flags);
    j.at(kPchObjectFlags).get_to(schema.pch_object_flags);
    j.at(kAsmCompileFlags).get_to(schema.asm_compile_flags);
    j.at(kCCompileFlags).get_to(schema.c_compile_flags);
    j.at(kCppCompileFlags).get_to(schema.cpp_compile_flags);
    j.at(kLinkFlags).get_to(schema.link_flags);

    j.at(kCompileDependencies).get_to(schema.internal_compile_dependencies);
    j.at(kLinkDependencies).get_to(schema.internal_link_dependencies);
    j.at(kPchCompiled).get_to(schema.pch_compiled);
    j.at(kTargetLinked).get_to(schema.target_linked);
  }
};

} // namespace buildcc::internal

#endif
