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

#ifndef BUILDCC_INCLUDE_REGISTER_H_
#define BUILDCC_INCLUDE_REGISTER_H_

#include <functional>
#include <unordered_map>

#include "args.h"
#include "target.h"

#include "taskflow/taskflow.hpp"

namespace buildcc {

class Register {
public:
  struct TestInfo {
    base::Target &target_;
    std::function<void(base::Target &target)> cb_;

    TestInfo(base::Target &target, std::function<void(base::Target &target)> cb)
        : target_(target), cb_(cb) {}
  };

public:
  Register(const Args &args) : args_(args) {}

  void Env();
  void Clean(std::function<void(void)> clean_cb);

  void Build(const Args::Toolchain &args_toolchain, base::Target &target,
             std::function<void(base::Target &)> build_cb);
  void Test(const Args::Toolchain &args_toolchain, base::Target &target,
            std::function<void(base::Target &)> test_cb);

  void Dep(const base::Target &target, const base::Target &dependency);

  void RunBuild();
  void RunTest();

  // Getters
  const tf::Taskflow &GetTaskflow() const { return taskflow_; }

private:
  const Args &args_;
  tf::Executor executor_;
  tf::Taskflow taskflow_{"Targets"};

  std::unordered_map<std::string, TestInfo> tests_;
  std::unordered_map<std::string, tf::Task> deps_;
};

} // namespace buildcc

#endif
