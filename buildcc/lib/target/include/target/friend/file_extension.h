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

#ifndef TARGET_FRIEND_FILE_EXTENSION_H_
#define TARGET_FRIEND_FILE_EXTENSION_H_

#include <filesystem>
#include <optional>

#include "fmt/format.h"

namespace fs = std::filesystem;

namespace buildcc::base {

class Target;

class FileExt {
public:
  enum class Type {
    Asm,
    C,
    Cpp,
    Header,
    Invalid,
  };

public:
  FileExt(Target &target) : target_(target) {}

  // Setters
  void SetSourceState(Type type);

  // Getters
  Type GetType(const fs::path &filepath) const;

  std::optional<std::string> GetCompileFlags(Type type) const;
  std::optional<std::string> GetCompiler(Type type) const;

  static std::string ToString(Type type);
  static bool IsValidSource(Type type);
  static bool IsValidHeader(Type type);

private:
  Target &target_;
};

} // namespace buildcc::base

// FMT specialization

template <>
struct fmt::formatter<buildcc::base::FileExt::Type> : formatter<std::string> {
  template <typename FormatContext>
  auto format(buildcc::base::FileExt::Type type, FormatContext &ctx) {
    return formatter<std::string>::format(
        buildcc::base::FileExt::ToString(type), ctx);
  }
};

#endif
