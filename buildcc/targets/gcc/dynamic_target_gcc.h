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

#ifndef TARGETS_GCC_DYNAMIC_TARGET_GCC_H_
#define TARGETS_GCC_DYNAMIC_TARGET_GCC_H_

#include "target.h"

namespace buildcc {

class DynamicTarget_gcc : public base::Target {
public:
  DynamicTarget_gcc(const std::string &name, const base::Toolchain &toolchain,
                    const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root) {}

private:
  virtual std::string_view CompileCommand() const {
    return "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} "
           "-fpic -o {output} -c {input}";
  }
  virtual std::string_view Link() const {
    return "{cpp_compiler} -shared {link_flags} {compiled_sources} -o {output}";
  }
};

} // namespace buildcc

#endif
