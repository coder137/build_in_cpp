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

#ifndef ARGS_REGISTER_TEST_INFO_H_
#define ARGS_REGISTER_TEST_INFO_H_

#include <filesystem>
#include <unordered_map>

#include "env/optional.h"

#include "target/target.h"

namespace fs = std::filesystem;

namespace buildcc {

struct TestOutput {
  enum class Type {
    DefaultBehaviour,  ///< Do not redirect to user or tests, default printed on
                       ///< console
    TestPrintOnStderr, ///< Test only redirects stderr and prints
    TestPrintOnStdout, ///< Test only redirects stdout and prints
    TestPrintOnStderrAndStdout, ///< Test redirects both and prints
    UserRedirect,               ///< Redirects to user variables
  };

  /**
   * @brief Configure your `Reg::Instance::Test` to get test output
   *
   * @param output_type Select your output type (behaviour)
   * @param redirect_stdout User stdout redirection
   * @param redirect_stderr User stderr redirection
   */
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
  /**
   * @brief Configure your `Reg::Instance::Test` using TestConfig
   *
   * @param arguments fmt::format args passed to test commands
   * @param working_directory Working directory from which the test runs
   * @param output Output from tests
   */
  // ! FIXME, warning: base class ‘struct
  // tl::detail::optional_operations_base<std::filesystem::__cxx11::path>’
  // should be explicitly initialized in the copy constructor
  TestConfig(
      const std::unordered_map<const char *, std::string> &arguments = {},
      const env::optional<fs::path> &working_directory = {},
      const TestOutput &output = TestOutput())
      : arguments_(arguments), working_directory_(working_directory),
        output_(output) {}

  const std::unordered_map<const char *, std::string> &GetArguments() const {
    return arguments_;
  }
  const env::optional<fs::path> &GetWorkingDirectory() const {
    return working_directory_;
  }
  const TestOutput &GetTestOutput() const { return output_; }

private:
  std::unordered_map<const char *, std::string> arguments_;
  env::optional<fs::path> working_directory_;
  TestOutput output_;
};

// PRIVATE

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
