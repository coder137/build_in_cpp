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

#include "args/register.h"

#include <filesystem>
#include <queue>

#include "fmt/format.h"

#include "env/assert_fatal.h"
#include "env/env.h"

namespace fs = std::filesystem;

namespace {
constexpr const char *const kRegkNotInit =
    "Initialize Reg using the Reg::Init API";
}

namespace {

void DepDetectDuplicate(const tf::Task &target_task, const std::string &match) {
  target_task.for_each_dependent([&](const tf::Task &t) {
    buildcc::env::log_trace("for_each_dependent", t.name());
    buildcc::env::assert_fatal(
        !(t.name() == match),
        fmt::format("Dependency '{}' already added", t.name()));
  });
}

void DepDetectCyclicDependency(const tf::Task &target_task,
                               const std::string &match) {
  std::queue<tf::Task> taskqueue;
  taskqueue.push(target_task);

  while (!taskqueue.empty()) {
    tf::Task current_task = taskqueue.front();
    taskqueue.pop();

    current_task.for_each_successor([&](const tf::Task &t) {
      buildcc::env::log_trace("for_each_successor", t.name());
      taskqueue.push(t);
      buildcc::env::assert_fatal(
          !(t.name() == match),
          fmt::format("Cyclic dependency detected when adding '{}'", t.name()));
    });
  }
}

} // namespace

namespace buildcc {

bool Reg::is_init_{false};
Register Reg::reg_;

void Reg::Init() {
  if (!is_init_) {
    env::assert_fatal(Args::IsParsed(), "Setup your Args");
    Project::Init(fs::current_path() / Args::GetProjectRootDir(),
                  fs::current_path() / Args::GetProjectBuildDir());
    env::set_log_level(Args::GetLogLevel());
    is_init_ = true;
  }
}

void Reg::Run() {
  reg_.RunBuild();
  reg_.RunTest();
}

Reg::ToolchainInstance Reg::Toolchain(const ArgToolchainState &condition) {
  env::assert_fatal(is_init_, kRegkNotInit);
  return ToolchainInstance(condition);
}

Reg::ToolchainInstance &
Reg::ToolchainInstance::Dep(const internal::BuilderInterface &target,
                            const internal::BuilderInterface &dependency) {
  reg_.Dep(target, dependency);
  return *this;
}

Reg::ToolchainInstance &Reg::ToolchainInstance::Test(const std::string &command,
                                                     const BaseTarget &target,
                                                     const TestConfig &config) {
  reg_.Test(condition_, command, target, config);
  return *this;
}

Reg::CallbackInstance Reg::Call(bool condition) {
  env::assert_fatal(is_init_, kRegkNotInit);
  return CallbackInstance(condition);
}

Reg::CallbackInstance &
Reg::CallbackInstance::Func(const std::function<void(void)> &cb) {
  if (condition_ && cb) {
    cb();
  }
  return *this;
}

void Register::Clean(const std::function<void(void)> &clean_cb) {
  if (Args::Clean()) {
    clean_cb();
  }
}

void Register::Dep(const internal::BuilderInterface &target,
                   const internal::BuilderInterface &dependency) {
  const auto target_iter = build_.find(target.GetUniqueId());
  const auto dep_iter = build_.find(dependency.GetUniqueId());
  env::assert_fatal(!(target_iter == build_.end() || dep_iter == build_.end()),
                    "Call Register::Build API on target and "
                    "dependency before Register::Dep API");

  //  empty tasks -> not built so skip
  if (target_iter->second.empty() || dep_iter->second.empty()) {
    return;
  }

  const std::string &dep_unique_id = dependency.GetUniqueId();
  DepDetectDuplicate(target_iter->second, dep_unique_id);
  DepDetectCyclicDependency(target_iter->second, dep_unique_id);

  // Finally do this
  target_iter->second.succeed(dep_iter->second);
}

void Register::Test(const ArgToolchainState &toolchain_state,
                    const std::string &command, const BaseTarget &target,
                    const TestConfig &config) {
  if (!(toolchain_state.build && toolchain_state.test)) {
    return;
  }

  const auto target_iter = build_.find(target.GetUniqueId());
  env::assert_fatal(
      !(target_iter == build_.end()),
      "Call Register::Build API on target before Register::Test API");

  const bool added =
      tests_.emplace(target.GetUniqueId(), TestInfo(target, command, config))
          .second;
  env::assert_fatal(
      added, fmt::format("Could not register test {}", target.GetName()));
}

// Private

void Register::BuildStoreTask(const std::string &unique_id,
                              const tf::Task &task) {
  const bool stored = build_.emplace(unique_id, task).second;
  env::assert_fatal(
      stored, fmt::format("Duplicate `Register::Build` call detected for '{}'",
                          unique_id));
}

//

void TestInfo::TestRunner() const {
  env::log_info(__FUNCTION__,
                fmt::format("Testing \'{}\'", target_.GetUniqueId()));
  env::Command command;
  command.AddDefaultArguments({
      {"executable", fmt::format("{}", target_.GetTargetPath())},
  });
  const std::string test_command =
      command.Construct(command_, config_.GetArguments());

  // TODO, Shift this to a function
  std::vector<std::string> test_redirect_stdout;
  std::vector<std::string> test_redirect_stderr;

  std::vector<std::string> *redirect_stdout{nullptr};
  std::vector<std::string> *redirect_stderr{nullptr};
  switch (config_.GetTestOutput().GetType()) {
  case TestOutput::Type::DefaultBehaviour:
    (void)test_redirect_stdout;
    (void)test_redirect_stderr;
    break;
  case TestOutput::Type::TestPrintOnStderr:
    redirect_stderr = &test_redirect_stderr;
    (void)test_redirect_stdout;
    break;
  case TestOutput::Type::TestPrintOnStdout:
    redirect_stdout = &test_redirect_stdout;
    (void)test_redirect_stderr;
    break;
  case TestOutput::Type::TestPrintOnStderrAndStdout:
    redirect_stdout = &test_redirect_stdout;
    redirect_stderr = &test_redirect_stderr;
    break;
  case TestOutput::Type::UserRedirect:
    redirect_stdout = config_.GetTestOutput().GetRedirectStdoutToUser();
    redirect_stderr = config_.GetTestOutput().GetRedirectStderrToUser();
    (void)test_redirect_stdout;
    (void)test_redirect_stderr;
    break;
  default:
    (void)test_redirect_stdout;
    (void)test_redirect_stderr;
    env::assert_fatal<false>("Invalid TestOutput::Type");
    break;
  };

  const bool success =
      env::Command::Execute(test_command, config_.GetWorkingDirectory(),
                            redirect_stdout, redirect_stderr);
  env::assert_fatal(success,
                    fmt::format("Could not run {}", target_.GetUniqueId()));

  // Print
  switch (config_.GetTestOutput().GetType()) {
  case TestOutput::Type::TestPrintOnStderr:
    env::log_info(fmt::format("STDERR: {}", target_.GetUniqueId()),
                  internal::aggregate(*redirect_stderr));
    break;
  case TestOutput::Type::TestPrintOnStdout:
    env::log_info(fmt::format("STDOUT: {}", target_.GetUniqueId()),
                  internal::aggregate(*redirect_stdout));
    break;
  case TestOutput::Type::TestPrintOnStderrAndStdout:
    env::log_info(fmt::format("STDOUT: {}", target_.GetUniqueId()),
                  internal::aggregate(*redirect_stdout));
    env::log_info(fmt::format("STDERR: {}", target_.GetUniqueId()),
                  internal::aggregate(*redirect_stderr));
    break;
  default:
    break;
  }
}

} // namespace buildcc
