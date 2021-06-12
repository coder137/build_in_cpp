/*
 * Copyright 2021 Niket Naidu All rights reserved.
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

#include "static_target_gcc.h"

#include "internal/util.h"

// Env
#include "assert_fatal.h"

namespace buildcc {

std::vector<std::string>
StaticTarget_gcc::Link(const std::string &output_target,
                       const std::string &aggregated_link_flags,
                       const std::string &aggregated_compiled_sources,
                       const std::string &aggregated_lib_dirs,
                       const std::string &aggregated_lib_deps) const {
  (void)aggregated_link_flags;
  (void)aggregated_lib_dirs;
  (void)aggregated_lib_deps;
  return {
      GetToolchain().GetArchiver(),
      "rcs",
      output_target,
      aggregated_compiled_sources,
  };
}

} // namespace buildcc
