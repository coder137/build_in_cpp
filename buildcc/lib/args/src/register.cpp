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

#include "args/register.h"

#include <filesystem>
#include <queue>

#include "fmt/format.h"

#include "env/assert_fatal.h"
#include "env/env.h"

namespace fs = std::filesystem;

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

void Register::Clean(const std::function<void(void)> &clean_cb) {
  if (args_.Clean()) {
    clean_cb();
  }
}

void Register::Dep(const base::Target &target, const base::Target &dependency) {
  const auto target_iter = store_.find(target.GetUniqueId());
  const auto dep_iter = store_.find(dependency.GetUniqueId());
  env::assert_fatal(!(target_iter == store_.end() || dep_iter == store_.end()),
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

void Register::Test(
    const Args::ToolchainState &toolchain_state, const std::string &command,
    base::Target &target,
    const std::unordered_map<const char *, std::string> &arguments = {}) {
  if (!(toolchain_state.build && toolchain_state.test)) {
    return;
  }

  const auto target_iter = store_.find(target.GetUniqueId());
  if (target_iter == store_.end()) {
    env::assert_fatal<false>(
        "Call Register::Build API on target before Register::Test API");
  }

  const bool added =
      tests_.emplace(target.GetUniqueId(), TestInfo(target, command, arguments))
          .second;
  env::assert_fatal(
      added, fmt::format("Could not register test {}", target.GetName()));
}

void Register::RunTest() {
  for (const auto &t : tests_) {
    env::log_info(__FUNCTION__, fmt::format("Testing \'{}\'",
                                            t.second.target_.GetUniqueId()));
    Command command;
    command.AddDefaultArguments({
        {"executable", t.second.target_.GetTargetPath().string()},
    });
    command.Construct(t.second.command_, t.second.arguments_);
  }
}

// Private

void Register::Initialize() { Env(); }

void Register::Env() {
  env::init(fs::current_path() / args_.GetProjectRootDir(),
            fs::current_path() / args_.GetProjectBuildDir());
  env::set_log_level(args_.GetLogLevel());
}

void Register::StoreTarget(const base::Target &target, const tf::Task &task) {
  const bool stored = store_.emplace(target.GetUniqueId(), task).second;
  env::assert_fatal(
      stored,
      fmt::format("Duplicate `Register::Build` call detected for target '{}'",
                  target.GetUniqueId()));
}

} // namespace buildcc
