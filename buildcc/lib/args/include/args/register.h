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

#ifndef ARGS_REGISTER_H_
#define ARGS_REGISTER_H_

#include <functional>
#include <unordered_map>

#include "args.h"

#include "target/generator.h"
#include "target/target.h"

#include "args/register/test_info.h"

#include "taskflow/taskflow.hpp"

namespace buildcc {

class Register {
public:
  Register() = default;
  Register(const Register &) = delete;

  /**
   * @brief Folders and files that need to be cleaned when `clean == true`
   */
  void Clean(const std::function<void(void)> &clean_cb);

  /**
   * @brief Generic register callback with variable arguments
   * Can be used to organize code into functional chunks
   */
  template <typename C, typename... Params>
  void Callback(const C &build_cb, Params &&...params) {
    build_cb(std::forward<Params>(params)...);
  }

  /**
   * @brief Generic register callback that is run when `expression ==
   * true`
   * Can be used to add Toolchain-Target specific information
   */
  template <typename C, typename... Params>
  void CallbackIf(bool expression, const C &build_cb, Params &&...params) {
    if (expression) {
      Callback(build_cb, std::forward<Params>(params)...);
    }
  }

  /**
   * @brief Generic register callback that is run when `toolchain_state.build ==
   * true`
   * Can be used to add Toolchain-Target specific information
   */
  template <typename C, typename... Params>
  void CallbackIf(const ArgToolchainState &toolchain_state, const C &build_cb,
                  Params &&...params) {
    CallbackIf(toolchain_state.build, build_cb,
               std::forward<Params>(params)...);
  }

  /**
   * @brief Register the Target to be built
   */
  template <typename C, typename... Params>
  void Build(const ArgToolchainState &toolchain_state, const C &build_cb,
             BaseTarget &target, Params &&...params) {
    tf::Task task;
    CallbackIf(
        toolchain_state,
        [&](BaseTarget &ltarget, Params &&...lparams) {
          build_cb(ltarget, std::forward<Params>(lparams)...);
          task = BuildTargetTask(ltarget);
        },
        target, std::forward<Params>(params)...);
    BuildStoreTask(target.GetUniqueId(), task);
  }

  /**
   * @brief Register the generator to be built
   */
  template <typename C, typename... Params>
  void Build(const C &build_cb, BaseGenerator &generator, Params &&...params) {
    build_cb(generator, std::forward<Params>(params)...);
    tf::Task task = BuildGeneratorTask(generator);
    BuildStoreTask(generator.GetUniqueId(), task);
  }

  /**
   * @brief Setup dependency between 2 Targets
   * PreReq: Call `Register::Build` before calling `Register::Dep`
   *
   * Target runs after dependency is built
   */
  void Dep(const internal::BuilderInterface &target,
           const internal::BuilderInterface &dependency);

  /**
   * @brief Register the Target to be run
   * PreReq: Call `Register::Build` before calling `Register::Test`
   * PreReq: Requires ArgToolchainState::build && ArgToolchainState::test to be
   * true
   *
   * Target is added as the `{executable}` argument.
   * We can add more fmt::format arguments using the TestConfig arguments
   * parameter
   */
  void Test(const ArgToolchainState &toolchain_state,
            const std::string &command, const BaseTarget &target,
            const TestConfig &config = TestConfig());

  /**
   * @brief Builds the targets that have been dynamically added through
   * `Register::Build`
   */
  void RunBuild();

  /**
   * @brief Runs the targets that have been dynamically added through
   * `Register::Test`
   */
  void RunTest();

  // TODO, Add a function to create Taskflow .dot dump into file or string

  // Getters
  const tf::Taskflow &GetTaskflow() const { return build_tf_; }

private:
private:
  // BuildTasks
  tf::Task BuildTargetTask(BaseTarget &target);
  tf::Task BuildGeneratorTask(BaseGenerator &generator);
  void BuildStoreTask(const std::string &unique_id, const tf::Task &task);

private:
  // Build
  tf::Taskflow build_tf_{"Targets"};

  std::unordered_map<std::string, tf::Task> build_;

  // Tests
  std::unordered_map<std::string, TestInfo> tests_;

  //
  tf::Executor executor_;
};

class Reg {
private:
  class ToolchainInstance {
  public:
    ToolchainInstance(const ArgToolchainState &condition)
        : condition_(condition) {}

    template <typename C, typename... Params>
    ToolchainInstance &Func(const C &cb, Params &&...params) {
      if (condition_.build) {
        cb(std::forward<Params>(params)...);
      }
      return *this;
    }
    template <typename C, typename... Params>
    ToolchainInstance &Build(const C &build_cb, BaseTarget &target,
                             Params &&...params) {
      reg_.Build(condition_, build_cb, target, std::forward<Params>(params)...);
      return *this;
    }
    ToolchainInstance &Dep(const internal::BuilderInterface &target,
                           const internal::BuilderInterface &dependency);
    ToolchainInstance &Test(const std::string &command,
                            const BaseTarget &target,
                            const TestConfig &config = TestConfig());

  private:
    ArgToolchainState condition_;
  };

  class CallbackInstance {
  public:
    CallbackInstance(bool condition = true) : condition_(condition) {}
    CallbackInstance &Func(const std::function<void(void)> &cb);

  private:
    bool condition_;
  };

public:
  static void Init();
  static void Run();
  static CallbackInstance Call(bool condition = true);
  static ToolchainInstance Toolchain(const ArgToolchainState &condition);

private:
  static bool is_init_;
  static Register reg_;
};

} // namespace buildcc

#endif
