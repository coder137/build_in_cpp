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

#include "target/custom_generator.h"
#include "target/file_generator.h"
#include "target/target.h"

#include "args/register/test_info.h"

#include "taskflow/taskflow.hpp"

namespace buildcc {

class Reg {
private:
  class Instance;
  class CallbackInstance;
  class ToolchainInstance;

public:
  Reg() = delete;
  Reg(const Reg &) = delete;
  Reg(Reg &&) = delete;

  static void Init();
  static void Deinit();
  static void Run(const std::function<void(void)> &post_build_cb =
                      std::function<void(void)>());
  static CallbackInstance Call(bool condition = true);
  static ToolchainInstance Toolchain(const ArgToolchainState &condition);

  static const tf::Taskflow &GetTaskflow();

private:
  static Instance &Ref();

private:
  static std::unique_ptr<Instance> instance_;
};

class Reg::Instance {
public:
  /**
   * @brief Generic register callback with variable arguments
   * Can be used to organize code into functional chunks
   */
  template <typename C, typename... Params>
  static void Callback(const C &build_cb, Params &&...params) {
    build_cb(std::forward<Params>(params)...);
  }

  /**
   * @brief Generic register callback that is run when `expression ==
   * true`
   * Can be used to add Toolchain-Target specific information
   */
  template <typename C, typename... Params>
  static void CallbackIf(bool expression, const C &build_cb,
                         Params &&...params) {
    if (expression) {
      Callback(build_cb, std::forward<Params>(params)...);
    }
  }

  template <typename C, typename T, typename... Params>
  void Build(const C &build_cb, T &builder, Params &&...params) {
    constexpr bool is_supported_base = std::is_base_of_v<CustomGenerator, T> ||
                                       std::is_base_of_v<BaseTarget, T>;
    static_assert(
        is_supported_base,
        "Build only supports BaseTarget, CustomGenerator and derivatives");

    build_cb(builder, std::forward<Params>(params)...);
    tf::Task task = BuildTask(builder);
    BuildStoreTask(builder.GetUniqueId(), task);
  }

  /**
   * @brief Setup dependency between 2 Targets
   * PreReq: Call `Reg::Instance::Build` before calling `Reg::Instance::Dep`
   *
   * Target runs after dependency is built
   */
  void Dep(const internal::BuilderInterface &target,
           const internal::BuilderInterface &dependency);

  /**
   * @brief Instance the Target to be run
   * PreReq: Call `Reg::Instance::Build` before calling `Reg::Instance::Test`
   * PreReq: Requires ArgToolchainState::build && ArgToolchainState::test to be
   * true
   *
   * Target is added as the `{executable}` argument.
   * We can add more fmt::format arguments using the TestConfig arguments
   * parameter
   */
  void Test(const std::string &command, const BaseTarget &target,
            const TestConfig &config = TestConfig());

  /**
   * @brief Builds the targets that have been dynamically added through
   * `Reg::Instance::Build`
   */
  void RunBuild();

  /**
   * @brief Runs the targets that have been dynamically added through
   * `Reg::Instance::Test`
   */
  void RunTest();

  // TODO, Add a function to create Taskflow .dot dump into file or string

  // Getters
  const tf::Taskflow &GetTaskflow() const { return build_tf_; }

private:
private:
  // BuildTasks
  tf::Task BuildTask(BaseTarget &target);
  tf::Task BuildTask(CustomGenerator &generator);
  void BuildStoreTask(const std::string &unique_id, const tf::Task &task);

private:
  // Build
  tf::Taskflow build_tf_{"Targets"};

  std::unordered_map<std::string, tf::Task> build_;
  std::unordered_map<std::string, TestInfo> tests_;
};

class Reg::CallbackInstance {
public:
  CallbackInstance(bool condition = true) : condition_(condition) {}

  // Duplicated code
  template <typename C, typename... Params>
  CallbackInstance &Func(const C &cb, Params &&...params) {
    Instance::CallbackIf(condition_, cb, std::forward<Params>(params)...);
    return *this;
  }

  template <typename C, typename T, typename... Params>
  CallbackInstance &Build(const C &build_cb, T &builder, Params &&...params) {
    if (condition_) {
      Ref().Build(build_cb, builder, std::forward<Params>(params)...);
    };
    return *this;
  }

private:
  bool condition_;
};

class Reg::ToolchainInstance {
public:
  ToolchainInstance(const ArgToolchainState &condition)
      : condition_(condition) {}

  template <typename C, typename... Params>
  ToolchainInstance &Func(const C &cb, Params &&...params) {
    Instance::CallbackIf(condition_.build, cb, std::forward<Params>(params)...);
    return *this;
  }

  template <typename C, typename T, typename... Params>
  ToolchainInstance &Build(const C &build_cb, T &builder, Params &&...params) {
    if (condition_.build) {
      Ref().Build(build_cb, builder, std::forward<Params>(params)...);
    };
    return *this;
  }
  // TODO, Update/Change this
  template <typename P> ToolchainInstance &BuildPackage(P &package) {
    return Func([&]() { package.Setup(condition_); });
  }
  ToolchainInstance &Dep(const internal::BuilderInterface &target,
                         const internal::BuilderInterface &dependency);
  ToolchainInstance &Test(const std::string &command, const BaseTarget &target,
                          const TestConfig &config = TestConfig());

private:
  ArgToolchainState condition_;
};

} // namespace buildcc

#endif
