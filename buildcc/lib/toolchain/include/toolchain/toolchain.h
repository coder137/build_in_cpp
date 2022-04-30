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

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "toolchain/common/function_lock.h"
#include "toolchain/common/toolchain_config.h"
#include "toolchain/common/toolchain_executables.h"
#include "toolchain/common/toolchain_id.h"

#include "toolchain/api/flag_api.h"
#include "toolchain/api/toolchain_find.h"
#include "toolchain/api/toolchain_verify.h"

namespace buildcc {

// Base toolchain class
class Toolchain : public internal::FlagApi<Toolchain>,
                  public ToolchainFind<Toolchain>,
                  public ToolchainVerify<Toolchain> {
public:
  // TODO, Remove ToolchainId from here
  Toolchain(ToolchainId id, std::string_view name,
            const ToolchainExecutables &executables,
            const ToolchainConfig &config = ToolchainConfig())
      : id_(id), name_(name), executables_(executables), config_(config),
        lock_(false) {}

  [[deprecated]] Toolchain(ToolchainId id, std::string_view name,
                           std::string_view assembler,
                           std::string_view c_compiler,
                           std::string_view cpp_compiler,
                           std::string_view archiver, std::string_view linker,
                           const ToolchainConfig &config = ToolchainConfig())
      : Toolchain(id, name,
                  ToolchainExecutables(assembler, c_compiler, cpp_compiler,
                                       archiver, linker),
                  config) {}

  virtual ~Toolchain() = default;
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

protected:
  ToolchainId &RefId() { return id_; }
  std::string &RefName() { return name_; }
  ToolchainExecutables &RefExecutables() { return executables_; }
  ToolchainConfig &RefConfig() { return config_; }

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
  friend class internal::FlagApi<Toolchain>;
  friend class ToolchainVerify<Toolchain>;

private:
  ToolchainId id_;
  std::string name_;
  ToolchainExecutables executables_;
  ToolchainConfig config_;
  FunctionLock lock_;

  //
  UserSchema user_;
};

typedef Toolchain BaseToolchain;

} // namespace buildcc

#endif
