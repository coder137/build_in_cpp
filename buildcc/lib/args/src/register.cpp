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

namespace buildcc {

void Register::Clean(const std::function<void(void)> &clean_cb) {
  if (args_.Clean()) {
    clean_cb();
  }
}

void Register::Build(const Args::ToolchainState &toolchain_state,
                     base::Target &target,
                     const std::function<void(base::Target &)> &build_cb) {
  tf::Task task;
  if (toolchain_state.build) {
    build_cb(target);
    task = BuildTask(target);
  }
  const bool target_stored =
      targets_.store.emplace(target.GetBinaryPath(), task).second;
  env::assert_fatal(target_stored, fmt::format("Could not register target {}",
                                               target.GetName()));
}

void Register::Dep(const base::Target &target, const base::Target &dependency) {
  //  empty tasks -> not built so skip
  const auto target_iter = targets_.store.find(target.GetBinaryPath());
  const auto dep_iter = targets_.store.find(dependency.GetBinaryPath());
  if (target_iter == targets_.store.end() || dep_iter == targets_.store.end()) {
    env::assert_fatal<false>("Call Register::Build API on target and "
                             "dependency before Register::Dep API");
  }
  if (target_iter->second.empty() || dep_iter->second.empty()) {
    return;
  }
  std::string deppath = dependency.GetTargetPath()
                            .lexically_relative(env::get_project_build_dir())
                            .string();
  std::replace(deppath.begin(), deppath.end(), '\\', '/');

  // DONE, Detect already added dependency
  target_iter->second.for_each_dependent([&](const tf::Task &t) {
    env::log_trace("for_each_dependent", t.name());
    if (t.name() == deppath) {
      env::assert_fatal<false>("Dependency already added");
    }
  });

  // DONE, Detect cyclic dependency
  std::queue<tf::Task> taskqueue;
  taskqueue.push(target_iter->second);

  while (!taskqueue.empty()) {
    tf::Task current_task = taskqueue.front();
    taskqueue.pop();

    current_task.for_each_successor([&](const tf::Task &t) {
      env::log_trace("for_each_successor", t.name());
      taskqueue.push(t);
      if (t.name() == deppath) {
        env::assert_fatal<false>("Cyclic dependency detected");
      }
    });
  }

  target_iter->second.succeed(dep_iter->second);
}

void Register::Test(const Args::ToolchainState &toolchain_state,
                    base::Target &target,
                    const std::function<void(base::Target &)> &test_cb) {
  if (!(toolchain_state.build && toolchain_state.test)) {
    return;
  }

  const auto target_iter = targets_.store.find(target.GetBinaryPath());
  if (target_iter == targets_.store.end()) {
    env::assert_fatal<false>(
        "Call Register::Build API on target before Register::Test API");
  }

  const bool added =
      tests_.emplace(target.GetBinaryPath(), TestInfo(target, test_cb)).second;
  env::assert_fatal(
      added, fmt::format("Could not register test {}", target.GetName()));
}

void Register::RunTest() {
  for (const auto &t : tests_) {
    env::log_info(__FUNCTION__,
                  fmt::format("Testing \'{}\'", t.first.string()));
    t.second.cb_(t.second.target_);
  }
}

// Private

void Register::Initialize() { Env(); }

void Register::Env() {
  env::init(fs::current_path() / args_.GetProjectRootDir(),
            fs::current_path() / args_.GetProjectBuildDir());
  env::set_log_level(args_.GetLogLevel());
}

} // namespace buildcc
