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

  void BuildTarget(bool build, base::Target &target,
                   std::function<void(base::Target &)> build_cb);
  void TestTarget(bool test, base::Target &target,
                  std::function<void(base::Target &)> test_cb);

  void BuildAll();
  void TestAll();

  // Getters
  const tf::Taskflow &GetTaskflow() const { return taskflow_; }

private:
  const Args &args_;
  tf::Executor executor_;
  tf::Taskflow taskflow_{"Targets"};

  std::unordered_map<std::string, TestInfo> tests_;
};

} // namespace buildcc

#endif
