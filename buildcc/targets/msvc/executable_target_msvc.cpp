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

#include "executable_target_msvc.h"

#include "internal/util.h"

// Env
#include "assert_fatal.h"

#include "fmt/format.h"

namespace buildcc {

// Compiling
std::vector<std::string> ExecutableTarget_msvc::CompileCommand(
    const std::string &input_source, const std::string &output_source,
    const std::string &compiler,
    const std::string &aggregated_preprocessor_flags,
    const std::string &aggregated_compile_flags,
    const std::string &aggregated_include_dirs) const {
  return {
      compiler,
      aggregated_preprocessor_flags,
      aggregated_include_dirs,
      aggregated_compile_flags,
      fmt::format("/Fo{}", output_source),
      "/c",
      input_source,
  };
}

// Linking
std::vector<std::string>
ExecutableTarget_msvc::Link(const std::string &output_target,
                            const std::string &aggregated_link_flags,
                            const std::string &aggregated_compiled_sources,
                            const std::string &aggregated_lib_dirs,
                            const std::string &aggregated_lib_deps) const {
  // clang-format off
  return {
      GetToolchain().GetLinker(),
      aggregated_link_flags,
      aggregated_lib_dirs,
      fmt::format("/OUT:{}", output_target),
      aggregated_lib_deps,
      aggregated_compiled_sources,
  };
  // clang-format on
}

} // namespace buildcc
