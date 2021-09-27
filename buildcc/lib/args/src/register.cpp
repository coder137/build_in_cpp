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
  if (toolchain_state.build) {
    build_cb(target);

    tf::Task task = BuildTask(target);
    deps_.relation.emplace(target.GetBinaryPath(), task);
  }
}

void Register::Dep(const base::Target &target, const base::Target &dependency) {
  // target_task / dep_task cannot be empty
  // Either present or not found
  const auto target_iter = deps_.relation.find(target.GetBinaryPath());
  const auto dep_iter = deps_.relation.find(dependency.GetBinaryPath());
  if (target_iter == deps_.relation.end() || dep_iter == deps_.relation.end()) {
    env::assert_fatal<false>("Call Register::Build API on target and "
                             "dependency before Register::Dep API");
  }
  target_iter->second.succeed(dep_iter->second);
}

void Register::Test(const Args::ToolchainState &toolchain_state,
                    base::Target &target,
                    const std::function<void(base::Target &)> &test_cb) {
  if (!(toolchain_state.build && toolchain_state.test)) {
    return;
  }

  const auto target_iter = deps_.relation.find(target.GetBinaryPath());
  if (target_iter == deps_.relation.end()) {
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
