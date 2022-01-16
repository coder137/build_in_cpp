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

#ifndef TOOLCHAIN_TOOLCHAIN_H_
#define TOOLCHAIN_TOOLCHAIN_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "api/toolchain_verify.h"

namespace buildcc {

// Base toolchain class
class Toolchain : public ToolchainVerify<Toolchain> {
public:
  enum class Id {
    Gcc = 0,
    Msvc,
    Clang,
    MinGW,
    Custom,
    Undefined,
  };

public:
  explicit Toolchain(Id id, std::string_view name,
                     std::string_view asm_compiler, std::string_view c_compiler,
                     std::string_view cpp_compiler, std::string_view archiver,
                     std::string_view linker)
      : id_(id), name_(name), asm_compiler_(asm_compiler),
        c_compiler_(c_compiler), cpp_compiler_(cpp_compiler),
        archiver_(archiver), linker_(linker) {}

  Toolchain(Toolchain &&toolchain) = default;
  Toolchain(const Toolchain &toolchain) = delete;

  // Getters
  Id GetId() const { return id_; }
  const std::string &GetName() const { return name_; }
  const std::string &GetAsmCompiler() const { return asm_compiler_; }
  const std::string &GetCCompiler() const { return c_compiler_; }
  const std::string &GetCppCompiler() const { return cpp_compiler_; }
  const std::string &GetArchiver() const { return archiver_; }
  const std::string &GetLinker() const { return linker_; }

private:
  friend class ToolchainVerify<Toolchain>;

private:
  Id id_;
  std::string name_;
  std::string asm_compiler_;
  std::string c_compiler_;
  std::string cpp_compiler_;
  std::string archiver_;
  std::string linker_;
};

typedef Toolchain::Id ToolchainId;
typedef Toolchain BaseToolchain;

} // namespace buildcc

#endif
