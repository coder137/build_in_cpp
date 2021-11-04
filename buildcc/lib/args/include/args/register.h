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

#ifndef ARGS_REGISTER_H_
#define ARGS_REGISTER_H_

#include <functional>
#include <unordered_map>

#include "args.h"
#include "target/target.h"

#include "taskflow/taskflow.hpp"

namespace buildcc {

class Register {
public:
  Register(const Args &args) : args_(args) { Initialize(); }
  Register(const Register &) = delete;

  void Clean(const std::function<void(void)> &clean_cb);

  /**
   * @brief Register the Target to be built
   *
   * @param toolchain_state `build state` registers the target
   * @param build_cb custom user callback to setup target build requirements
   * @param target target taskflow is passed to build_cb (passed as reference)
   * @param params Additional parameters (passed as reference)
   */
  template <typename C, typename... Params>
  void Build(const Args::ToolchainState &toolchain_state, const C &build_cb,
             base::Target &target, Params &...params) {
    tf::Task task;
    if (toolchain_state.build) {
      build_cb(target, std::forward<Params &>(params)...);
      task = BuildTask(target);
    }
    StoreTarget(target, task);
  }

  /**
   * @brief Register the Target to be run
   * PreReq: Call `Register::Build` before calling `Register::Test`
   *
   * @param toolchain_state `build and test state` registers the target for
   * testing
   * @param command Command string pattern
   * `{executable}` is the built executable which is always added to the pattern
   *
   * @param target Target added as the `{executable}`
   *
   * @param arguments Addition arguments to be added to the command string
   * pattern
   */
  void
  Test(const Args::ToolchainState &toolchain_state, const std::string &command,
       base::Target &target,
       const std::unordered_map<const char *, std::string> &arguments = {});

  /**
   * @brief Setup dependency between 2 Targets
   * PreReq: Call `Register::Build` before calling `Register::Dep`
   *
   * @param target
   * @param dependency
   * Target runs after dependency is built
   *
   */
  void Dep(const base::Target &target, const base::Target &dependency);

  void RunBuild();
  void RunTest();

  // Getters
  const tf::Taskflow &GetBuildTaskflow() const { return build_tf_; }

private:
  struct TestInfo {
    TestInfo(base::Target &target, const std::string &command,
             const std::unordered_map<const char *, std::string> &arguments)
        : target_(target), command_(command), arguments_(arguments) {}

    void ConstructTestRunner() const;

    base::Target &target_;
    std::string command_;
    std::unordered_map<const char *, std::string> arguments_;
  };

private:
  void Initialize();

  // Setup env:: defaults
  void Env();

  //
  tf::Task BuildTask(base::Target &target);

  //
  void StoreTarget(const base::Target &target, const tf::Task &task);

private:
  const Args &args_;

  // Build
  tf::Taskflow build_tf_{"Targets"};
  std::unordered_map<std::string, tf::Task> build_;

  // Tests
  tf::Taskflow test_tf_{"Tests"};
  std::unordered_map<std::string, TestInfo> tests_;

  //
  tf::Executor executor_;
};

} // namespace buildcc

#endif
