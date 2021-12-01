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

#include "bootstrap/buildcc_flatbuffers.h"

namespace {

const std::vector<std::string> kFlatcSources{
    "code_generators.cpp",
    "flatc_main.cpp",
    "flatc.cpp",
    "idl_gen_cpp.cpp",
    "idl_gen_csharp.cpp",
    "idl_gen_dart.cpp",
    "idl_gen_fbs.cpp",
    "idl_gen_go.cpp",
    "idl_gen_grpc.cpp",
    "idl_gen_java.cpp",
    "idl_gen_json_schema.cpp",
    "idl_gen_kotlin.cpp",
    "idl_gen_lobster.cpp",
    "idl_gen_lua.cpp",
    "idl_gen_php.cpp",
    "idl_gen_python.cpp",
    "idl_gen_rust.cpp",
    "idl_gen_swift.cpp",
    "idl_gen_text.cpp",
    "idl_gen_ts.cpp",
    "idl_parser.cpp",
    "reflection.cpp",
    "util.cpp",
};

const std::vector<std::string> kFlatcPreprocessorFlags{
    "-DFLATBUFFERS_LOCALE_INDEPENDENT=0",
    "-DNDEBUG",
};

const std::vector<std::string> kFlatcGccCppCompileFlags{
    "-pedantic",          "-Werror=shadow",
    "-faligned-new",      "-Werror=implicit-fallthrough=2",
    "-Wunused-result",    "-Werror=unused-result",
    "-Wunused-parameter", "-Werror=unused-parameter",
    "-fsigned-char",      "-Wold-style-cast",
    "-Winvalid-pch",
};

const std::vector<std::string> kFlatcMsvcCppCompileFlags{
    "/MP",         "/MT",          "/GS",        "/fp:precise",
    "/Zc:wchar_t", "/Zc:forScope", "/Zc:inline", "/GR",
};

} // namespace

namespace buildcc {

void build_flatc_exe_cb(BaseTarget &target) {
  std::for_each(kFlatcSources.cbegin(), kFlatcSources.cend(),
                [&](const auto &s) { target.AddSource(s, "src"); });
  target.GlobSources("grpc/src/compiler");

  target.AddIncludeDir("include");
  target.AddIncludeDir("grpc");

  target.GlobHeaders("include/flatbuffers");
  target.GlobHeaders("grpc/src/compiler");

  std::for_each(kFlatcPreprocessorFlags.cbegin(),
                kFlatcPreprocessorFlags.cend(),
                [&](const auto &f) { target.AddPreprocessorFlag(f); });

  if constexpr (env::is_win()) {
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc:
    case ToolchainId::MinGW:
      std::for_each(kFlatcGccCppCompileFlags.cbegin(),
                    kFlatcGccCppCompileFlags.cend(),
                    [&](const auto &f) { target.AddCppCompileFlag(f); });
      break;
    case ToolchainId::Msvc:
      std::for_each(kFlatcMsvcCppCompileFlags.cbegin(),
                    kFlatcMsvcCppCompileFlags.cend(),
                    [&](const auto &f) { target.AddCppCompileFlag(f); });
      break;
    default:
      break;
    }
  }

  if constexpr (env::is_linux()) {
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc:
      std::for_each(kFlatcGccCppCompileFlags.cbegin(),
                    kFlatcGccCppCompileFlags.cend(),
                    [&](const auto &f) { target.AddCppCompileFlag(f); });
      break;
    default:
      break;
    }
  }

  // TODO, Add PCH

  target.Build();
}

void flatbuffers_ho_cb(TargetInfo &info) {
  info.AddIncludeDir("include");
  info.GlobHeaders("include/flatbuffers");
  // TODO, Add PCH
}

} // namespace buildcc
