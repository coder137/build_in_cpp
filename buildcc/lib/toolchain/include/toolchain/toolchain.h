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

#include "toolchain/common/function_lock.h"
#include "toolchain/common/toolchain_config.h"

#include "toolchain/api/flag_api.h"
#include "toolchain/api/toolchain_find.h"
#include "toolchain/api/toolchain_verify.h"

namespace buildcc {

enum class ToolchainId {
  Gcc = 0,   ///< GCC Toolchain
  Msvc,      ///< MSVC Toolchain
  Clang,     ///< Clang Toolchain
  MinGW,     ///< MinGW Toolchain (Similar to GCC, but for Windows)
  Custom,    ///< Custom Toolchain not defined in this list
  Undefined, ///< Default value when unknown
};

struct ToolchainExecutables {
  explicit ToolchainExecutables() = default;
  explicit ToolchainExecutables(std::string_view as, std::string_view c,
                                std::string_view cpp, std::string_view ar,
                                std::string_view link)
      : assembler(as), c_compiler(c), cpp_compiler(cpp), archiver(ar),
        linker(link) {}
  std::string assembler;
  std::string c_compiler;
  std::string cpp_compiler;
  std::string archiver;
  std::string linker;
};

// Base toolchain class
class Toolchain : public internal::FlagApi<Toolchain>,
                  public ToolchainFind<Toolchain>,
                  public ToolchainVerify<Toolchain> {
public:
public:
  Toolchain(ToolchainId id, std::string_view name,
            const ToolchainExecutables &binaries, bool lock = true,
            const ToolchainConfig &config = ToolchainConfig())
      : id_(id), name_(name), executables_(binaries), lock_(lock),
        config_(config) {
    Initialize();
  }
  Toolchain(ToolchainId id, std::string_view name, std::string_view assembler,
            std::string_view c_compiler, std::string_view cpp_compiler,
            std::string_view archiver, std::string_view linker,
            bool lock = true, const ToolchainConfig &config = ToolchainConfig())
      : Toolchain(id, name,
                  ToolchainExecutables(assembler, c_compiler, cpp_compiler,
                                       archiver, linker),
                  lock, config) {}

  Toolchain(Toolchain &&) = default;
  Toolchain &operator=(Toolchain &&) = default;
  Toolchain(const Toolchain &) = delete;
  Toolchain &operator=(const Toolchain &) = delete;

  void Lock();

  // Getters
  ToolchainId GetId() const { return id_; }
  const std::string &GetName() const { return name_; }
  const std::string &GetAssembler() const { return executables_.assembler; }
  const std::string &GetCCompiler() const { return executables_.c_compiler; }
  const std::string &GetCppCompiler() const {
    return executables_.cpp_compiler;
  }
  const std::string &GetArchiver() const { return executables_.archiver; }
  const std::string &GetLinker() const { return executables_.linker; }
  const ToolchainExecutables &GetToolchainExecutables() const {
    return executables_;
  }

  const FunctionLock &GetLockInfo() const { return lock_; }
  const ToolchainConfig &GetConfig() const { return config_; }

private:
  struct UserSchema {
    std::unordered_set<std::string> preprocessor_flags;
    std::unordered_set<std::string> common_compile_flags;
    std::unordered_set<std::string> pch_compile_flags;
    std::unordered_set<std::string> pch_object_flags;
    std::unordered_set<std::string> asm_compile_flags;
    std::unordered_set<std::string> c_compile_flags;
    std::unordered_set<std::string> cpp_compile_flags;
    std::unordered_set<std::string> link_flags;
  };

private:
  void Initialize();

  virtual void UpdateConfig(ToolchainConfig &config) { (void)config; }
  virtual std::optional<ToolchainCompilerInfo>
  VerifySelectedToolchainPath(const ToolchainExecutables &binaries) const {
    (void)binaries;
    return {};
  }

private:
  friend class internal::FlagApi<Toolchain>;
  friend class ToolchainVerify<Toolchain>;

private:
  ToolchainId id_;
  std::string name_;
  ToolchainExecutables executables_;
  FunctionLock lock_;
  ToolchainConfig config_;

  //
  UserSchema user_;
};

typedef Toolchain BaseToolchain;

} // namespace buildcc

#endif
