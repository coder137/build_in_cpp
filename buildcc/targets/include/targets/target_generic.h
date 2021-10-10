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

#ifndef TARGETS_TARGET_GENERIC_H_
#define TARGETS_TARGET_GENERIC_H_

#include <algorithm>

#include "target_gcc.h"
#include "target_msvc.h"

namespace buildcc {

struct CopyTarget {
  // TODO, Add other options
  enum class Option {
    PreprocessorFlags,
    CommonCompileFlags,
    AsmCompileFlags,
    CCompileFlags,
    CppCompileFlags,
    LinkFlags,
  };

  CopyTarget(base::Target &dest, const base::Target &source)
      : dest_(dest), source_(source) {}
  CopyTarget(const CopyTarget &target) = delete;
  CopyTarget(CopyTarget &&target) = delete;

  CopyTarget &Add(Option opt) {
    options_.insert(opt);
    return *this;
  }

  CopyTarget &Add(std::initializer_list<Option> opts) {
    options_.insert(opts.begin(), opts.end());
    return *this;
  }

  void Copy() {
    for (Option opt : options_) {
      switch (opt) {
      case Option::PreprocessorFlags:
        CopyCb<std::unordered_set<std::string>, std::string>(
            source_.GetCurrentPreprocessorFlags(),
            [&](const std::string &f) { dest_.AddPreprocessorFlag(f); });
        break;

      case Option::CommonCompileFlags:
        CopyCb<std::unordered_set<std::string>, std::string>(
            source_.GetCurrentCommonCompileFlags(),
            [&](const std::string &f) { dest_.AddCommonCompileFlag(f); });
        break;

      case Option::AsmCompileFlags:
        CopyCb<std::unordered_set<std::string>, std::string>(
            source_.GetCurrentAsmCompileFlags(),
            [&](const std::string &f) { dest_.AddAsmCompileFlag(f); });
        break;

      case Option::CCompileFlags:
        CopyCb<std::unordered_set<std::string>, std::string>(
            source_.GetCurrentCCompileFlags(),
            [&](const std::string &f) { dest_.AddCCompileFlag(f); });
        break;

      case Option::CppCompileFlags:
        CopyCb<std::unordered_set<std::string>, std::string>(
            source_.GetCurrentCppCompileFlags(),
            [&](const std::string &f) { dest_.AddCppCompileFlag(f); });
        break;

      case Option::LinkFlags:
        CopyCb<std::unordered_set<std::string>, std::string>(
            source_.GetCurrentLinkFlags(),
            [&](const std::string &f) { dest_.AddLinkFlag(f); });
        break;

      default:
        env::assert_fatal<false>("Invalid Option");
        break;
      }
    }
  }

private:
  template <typename list_type, typename var_type>
  void CopyCb(
      const list_type &copy_source_list,
      const std::function<void(const var_type &)> &cb = [](const var_type &) {
      }) {
    ASSERT_FATAL(cb, "Bad Function: cb");
    for (const var_type &s : copy_source_list) {
      cb(s);
    }
  }

private:
  const base::Target &source_;
  base::Target &dest_;
  std::unordered_set<Option> options_;
};

class GenericConfig {
public:
  static base::Target::Config Generic(base::TargetType type,
                                      const base::Toolchain &toolchain) {
    base::Target::Config config;
    switch (type) {
    case base::TargetType::Executable:
      config = Executable(toolchain);
      break;
    case base::TargetType::StaticLibrary:
      config = StaticLib(toolchain);
      break;
    case base::TargetType::DynamicLibrary:
      config = DynamicLib(toolchain);
      break;
    default:
      env::assert_fatal<false>("Target Type not supported");
      break;
    }

    return config;
  }

  static base::Target::Config Executable(const base::Toolchain &toolchain) {
    return DefaultGenericExecutable(toolchain);
  }
  static base::Target::Config StaticLib(const base::Toolchain &toolchain) {
    return DefaultGenericStaticLib(toolchain);
  }
  static base::Target::Config DynamicLib(const base::Toolchain &toolchain) {
    return DefaultGenericDynamicLib(toolchain);
  }

private:
  static base::Target::Config
  DefaultGenericExecutable(const base::Toolchain &toolchain) {
    base::Target::Config config;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      config = GccConfig::Executable();
      break;
    case base::Toolchain::Id::Msvc:
      config = MsvcConfig::Executable();
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }

  static base::Target::Config
  DefaultGenericStaticLib(const base::Toolchain &toolchain) {
    base::Target::Config config;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      config = GccConfig::StaticLib();
      break;
    case base::Toolchain::Id::Msvc:
      config = MsvcConfig::StaticLib();
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }

  static base::Target::Config
  DefaultGenericDynamicLib(const base::Toolchain &toolchain) {
    base::Target::Config config;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      config = GccConfig::DynamicLib();
      break;
    case base::Toolchain::Id::Msvc:
      config = MsvcConfig::DynamicLib();
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal<false>("Compiler ID not supported");
      break;
    }

    return config;
  }
};

class ExecutableTarget_generic : public base::Target {
public:
  ExecutableTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root,
               GenericConfig::Executable(toolchain)) {
    std::unique_ptr<base::Target> target;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      target = std::make_unique<ExecutableTarget_gcc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Msvc:
      target = std::make_unique<ExecutableTarget_msvc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }

    // Copy these parameters
    CopyTarget(*this, *target)
        .Add({
            CopyTarget::Option::CommonCompileFlags,
            CopyTarget::Option::AsmCompileFlags,
            CopyTarget::Option::CCompileFlags,
            CopyTarget::Option::CppCompileFlags,
            CopyTarget::Option::LinkFlags,
        })
        .Copy();
  }
  ~ExecutableTarget_generic() {}
};

class StaticTarget_generic : public base::Target {
public:
  StaticTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root,
               GenericConfig::StaticLib(toolchain)) {
    std::unique_ptr<base::Target> target;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      target = std::make_unique<StaticTarget_gcc>(name, toolchain,
                                                  target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Msvc:
      target = std::make_unique<StaticTarget_msvc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }
    // Copy these parameters
    CopyTarget(*this, *target)
        .Add({
            CopyTarget::Option::CommonCompileFlags,
            CopyTarget::Option::AsmCompileFlags,
            CopyTarget::Option::CCompileFlags,
            CopyTarget::Option::CppCompileFlags,
            CopyTarget::Option::LinkFlags,
        })
        .Copy();
  }
};

class DynamicTarget_generic : public base::Target {
public:
  DynamicTarget_generic(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root,
               GenericConfig::DynamicLib(toolchain)) {
    std::unique_ptr<base::Target> target;
    switch (toolchain.GetId()) {
    case base::Toolchain::Id::Gcc:
      target = std::make_unique<DynamicTarget_gcc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Msvc:
      target = std::make_unique<DynamicTarget_msvc>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::Toolchain::Id::Clang:
    case base::Toolchain::Id::MinGW:
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }
    // Copy these parameters
    CopyTarget(*this, *target)
        .Add({
            CopyTarget::Option::CommonCompileFlags,
            CopyTarget::Option::AsmCompileFlags,
            CopyTarget::Option::CCompileFlags,
            CopyTarget::Option::CppCompileFlags,
            CopyTarget::Option::LinkFlags,
        })
        .Copy();
  }
};

class Target_generic : public base::Target {
public:
  Target_generic(const std::string &name, base::TargetType type,
                 const base::Toolchain &toolchain,
                 const std::filesystem::path &target_path_relative_to_root)
      : Target(name, type, toolchain, target_path_relative_to_root,
               GenericConfig::Generic(type, toolchain)) {
    std::unique_ptr<base::Target> target;
    switch (type) {
    case base::TargetType::Executable:
      target = std::make_unique<ExecutableTarget_generic>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::TargetType::StaticLibrary:
      target = std::make_unique<StaticTarget_generic>(
          name, toolchain, target_path_relative_to_root);
      break;
    case base::TargetType::DynamicLibrary:
      target = std::make_unique<DynamicTarget_generic>(
          name, toolchain, target_path_relative_to_root);
      break;
    default:
      env::assert_fatal(false, "Compiler ID not supported");
      break;
    }
    // Copy these parameters
    CopyTarget(*this, *target)
        .Add({
            CopyTarget::Option::CommonCompileFlags,
            CopyTarget::Option::AsmCompileFlags,
            CopyTarget::Option::CCompileFlags,
            CopyTarget::Option::CppCompileFlags,
            CopyTarget::Option::LinkFlags,
        })
        .Copy();
  }
};

} // namespace buildcc

#endif
