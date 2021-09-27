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
   * @param target target taskflow is registered
   * @param build_cb custom user callback to setup target build requirements
   */
  void Build(const Args::ToolchainState &toolchain_state, base::Target &target,
             const std::function<void(base::Target &)> &build_cb);

  /**
   * @brief Register the Target to be run
   * PreReq: Call `Register::Build` before calling `Register::Test`
   *
   * @param toolchain_state `build and test state` registers the target for
   * testing
   * @param target target is registered for test
   * @param test_cb custom user callback for testing
   */
  void Test(const Args::ToolchainState &toolchain_state, base::Target &target,
            const std::function<void(base::Target &)> &test_cb);

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
  const tf::Taskflow &GetTaskflow() const { return targets_.tf; }

private:
  struct TestInfo {
    base::Target &target_;
    std::function<void(base::Target &target)> cb_;

    TestInfo(base::Target &target,
             const std::function<void(base::Target &target)> &cb)
        : target_(target), cb_(cb) {}
  };

  struct RegInfo {
    std::unordered_map<fs::path, tf::Task, internal::PathHash> store;
    tf::Taskflow tf;

    RegInfo(const std::string &name) : tf(name) {}
  };

private:
  void Initialize();

  // Setup env:: defaults
  void Env();

  void Dep(RegInfo &reginfo, const base::Target &target,
           const base::Target &dependency);

  //
  tf::Task BuildTask(base::Target &target);

private:
  const Args &args_;

  RegInfo targets_{"Targets"};
  RegInfo graphs_{"Graphs"};

  tf::Executor executor_;

  std::unordered_map<fs::path, TestInfo, internal::PathHash> tests_;
};

} // namespace buildcc

#endif
