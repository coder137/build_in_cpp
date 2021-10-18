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

#ifndef TARGET_GENERATOR_LOADER_H_
#define TARGET_GENERATOR_LOADER_H_

#include "target/loader_interface.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "fmt/format.h"

#include "target/path.h"

namespace buildcc::internal {

struct GenInfo {
  std::string name;
  default_files inputs;
  fs_unordered_set outputs;
  std::vector<std::string> commands;
  bool parallel{false};

  static GenInfo CreateInternalGenInfo(const std::string &n,
                                       const path_unordered_set &internal_i,
                                       const fs_unordered_set &o,
                                       const std::vector<std::string> &c,
                                       bool p) {
    return GenInfo(n, internal_i, {}, o, c, p);
  }

  static GenInfo CreateUserGenInfo(const std::string &n,
                                   const fs_unordered_set &user_i,
                                   const fs_unordered_set &o,
                                   const std::vector<std::string> &c, bool p) {
    return GenInfo(n, {}, user_i, o, c, p);
  }

  GenInfo(GenInfo &&info) = default;
  GenInfo(const GenInfo &info) = delete;

private:
  explicit GenInfo(const std::string &n, const path_unordered_set &internal_i,
                   const fs_unordered_set &user_i, const fs_unordered_set &o,
                   const std::vector<std::string> &c, bool p)
      : name(n), inputs(internal_i, user_i), outputs(o), commands(c),
        parallel(p) {}
};

typedef std::unordered_map<std::string, GenInfo> geninfo_unordered_map;

class GeneratorLoader : public LoaderInterface {
public:
  GeneratorLoader(const std::string &name, const fs::path &path)
      : name_(name), path_(path) {}

  GeneratorLoader(const GeneratorLoader &loader) = delete;

  bool Load() override;

  // Getters
  fs::path GetBinaryPath() const override {
    return path_ / fmt::format("{}.bin", name_);
  }

  const geninfo_unordered_map &GetLoadedInfo() { return loaded_info_; }

private:
  std::string name_;
  fs::path path_;

  geninfo_unordered_map loaded_info_;
};

} // namespace buildcc::internal

#endif
