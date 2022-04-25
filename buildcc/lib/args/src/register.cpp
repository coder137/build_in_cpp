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
#include "env/storage.h"

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

std::unique_ptr<Reg::Instance> Reg::instance_;

void SystemInit() {
  Project::Init(fs::current_path() / Args::GetProjectRootDir(),
                fs::current_path() / Args::GetProjectBuildDir());
  env::set_log_level(Args::GetLogLevel());

  // Top down (what is init first gets deinit last)
  std::atexit([]() {
    Project::Deinit();
    Reg::Deinit();
    Args::Deinit();
    Storage::Clear();
  });
}

void Reg::Init() {
  if (!instance_) {
    instance_ = std::make_unique<Instance>();
    env::assert_fatal(static_cast<bool>(instance_), "Reg::Init failed");
    env::assert_fatal(Args::IsParsed(), "Setup your Args");

    // Initialize everything else here
    SystemInit();
  }
}

void Reg::Deinit() {
  instance_.reset(nullptr);
  Project::Deinit();
}

void Reg::Run(const std::function<void(void)> &post_build_cb) {
  auto &ref = Ref();
  ref.RunBuild();
  if (post_build_cb) {
    post_build_cb();
  }
  ref.RunTest();
}

const tf::Taskflow &Reg::GetTaskflow() { return Ref().GetTaskflow(); }

Reg::Instance &Reg::Ref() {
  env::assert_fatal(instance_ != nullptr, kRegkNotInit);
  return *instance_;
}

// Reg::ToolchainInstance

Reg::ToolchainInstance Reg::Toolchain(const ArgToolchainState &condition) {
  env::assert_fatal(instance_ != nullptr, kRegkNotInit);
  return ToolchainInstance(condition);
}

Reg::ToolchainInstance &
Reg::ToolchainInstance::Dep(const internal::BuilderInterface &target,
                            const internal::BuilderInterface &dependency) {
  if (condition_.build) {
    Ref().Dep(target, dependency);
  }
  return *this;
}

Reg::ToolchainInstance &Reg::ToolchainInstance::Test(const std::string &command,
                                                     const BaseTarget &target,
                                                     const TestConfig &config) {
  if (condition_.build && condition_.test) {
    Ref().Test(command, target, config);
  }
  return *this;
}

// Reg::CallbackInstance

Reg::CallbackInstance Reg::Call(bool condition) {
  env::assert_fatal(instance_ != nullptr, kRegkNotInit);
  return CallbackInstance(condition);
}

// Reg::Instance

void Reg::Instance::Dep(const internal::BuilderInterface &target,
                        const internal::BuilderInterface &dependency) {
  const auto target_iter = build_.find(target.GetUniqueId());
  const auto dep_iter = build_.find(dependency.GetUniqueId());
  env::assert_fatal(!(target_iter == build_.end() || dep_iter == build_.end()),
                    "Call Instance::Build API on target and "
                    "dependency before Instance::Dep API");

  const std::string &dep_unique_id = dependency.GetUniqueId();
  DepDetectDuplicate(target_iter->second, dep_unique_id);
  DepDetectCyclicDependency(target_iter->second, dep_unique_id);

  // Finally do this
  target_iter->second.succeed(dep_iter->second);
}

void Reg::Instance::Test(const std::string &command, const BaseTarget &target,
                         const TestConfig &config) {
  const auto target_iter = build_.find(target.GetUniqueId());
  env::assert_fatal(
      !(target_iter == build_.end()),
      "Call Instance::Build API on target before Instance::Test API");

  const bool added =
      tests_.emplace(target.GetUniqueId(), TestInfo(target, command, config))
          .second;
  env::assert_fatal(
      added, fmt::format("Could not register test {}", target.GetName()));
}

// Private

void Reg::Instance::BuildStoreTask(const std::string &unique_id,
                                   const tf::Task &task) {
  const bool stored = build_.emplace(unique_id, task).second;
  env::assert_fatal(
      stored, fmt::format("Duplicate `Instance::Build` call detected for '{}'",
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
