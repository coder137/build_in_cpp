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

struct TestOutput {
  enum class Type {
    DefaultBehaviour,  // Do not redirect to user or tests, default printed on
                       // console
    TestPrintOnStderr, // Test only redirects stderr and prints
    TestPrintOnStdout, // Test only redirects stdout and prints
    TestPrintOnStderrAndStdout, // Test redirects both and prints
    UserRedirect,               // Redirects to user
  };

  TestOutput(Type output_type = Type::TestPrintOnStderrAndStdout,
             std::vector<std::string> *redirect_stdout = nullptr,
             std::vector<std::string> *redirect_stderr = nullptr)
      : type_(output_type), redirect_stdout_to_user_(redirect_stdout),
        redirect_stderr_to_user_(redirect_stderr) {}

  Type GetType() const { return type_; }
  std::vector<std::string> *GetRedirectStdoutToUser() const {
    return redirect_stdout_to_user_;
  }
  std::vector<std::string> *GetRedirectStderrToUser() const {
    return redirect_stderr_to_user_;
  }

private:
  Type type_;
  std::vector<std::string> *redirect_stdout_to_user_;
  std::vector<std::string> *redirect_stderr_to_user_;
};

struct TestConfig {
public:
  TestConfig(
      const std::unordered_map<const char *, std::string> &arguments = {},
      const std::optional<fs::path> &working_directory = {},
      const TestOutput &output = TestOutput())
      : arguments_(arguments), working_directory_(working_directory),
        output_(output) {}

  const std::unordered_map<const char *, std::string> &GetArguments() const {
    return arguments_;
  }
  const std::optional<fs::path> &GetWorkingDirectory() const {
    return working_directory_;
  }
  const TestOutput &GetTestOutput() const { return output_; }

private:
  std::unordered_map<const char *, std::string> arguments_;
  std::optional<fs::path> working_directory_{};
  TestOutput output_;
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
