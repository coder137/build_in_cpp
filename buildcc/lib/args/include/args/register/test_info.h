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

#ifndef ARGS_REGISTER_TEST_INFO_H_
#define ARGS_REGISTER_TEST_INFO_H_

#include "target/target.h"

#include <optional>
#include <unordered_map>

namespace buildcc {

struct TestConfig {
public:
  TestConfig(
      const std::unordered_map<const char *, std::string> &arguments = {},
      const std::optional<fs::path> &working_directory = {})
      : arguments_(arguments), working_directory_(working_directory) {}

  const std::unordered_map<const char *, std::string> &GetArguments() const {
    return arguments_;
  }
  const std::optional<fs::path> &GetWorkingDirectory() const {
    return working_directory_;
  }

private:
  std::unordered_map<const char *, std::string> arguments_;
  std::optional<fs::path> working_directory_{};
};

struct TestInfo {
  TestInfo(const BaseTarget &target, const std::string &command,
           const TestConfig &config = TestConfig())
      : target_(target), command_(command), config_(config) {}

  void TestRunner() const;

private:
  const BaseTarget &target_;
  std::string command_;
  TestConfig config_;
};

} // namespace buildcc

#endif
