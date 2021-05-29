#include "register.h"

#include <filesystem>

#include "fmt/format.h"

#include "assert_fatal.h"
#include "env.h"

namespace fs = std::filesystem;

namespace buildcc {

void Register::Env() {
  env::init(fs::current_path() / args_.GetProjectRootDir(),
            fs::current_path() / args_.GetProjectBuildDir());
  env::set_log_level(args_.GetLogLevel());
}

void Register::Clean(std::function<void(void)> clean_cb) {
  if (args_.Clean()) {
    clean_cb();
  }
}

void Register::Build(const Args::Toolchain &args_toolchain,
                     base::Target &target,
                     std::function<void(base::Target &)> build_cb) {
  if (args_toolchain.build) {
    tf::Task task = taskflow_.composed_of(target.GetTaskflow()).name("Task");
    deps_.insert({target.GetName(), task});
    build_cb(target);
  }
}

void Register::Test(const Args::Toolchain &args_toolchain, base::Target &target,
                    std::function<void(base::Target &)> test_cb) {
  if (!(args_toolchain.build && args_toolchain.test)) {
    return;
  }

  const bool added =
      tests_.emplace(target.GetName(), TestInfo(target, test_cb)).second;
  env::assert_fatal(
      added, fmt::format("Could not register test {}", target.GetName()));
}

void Register::Dep(const base::Target &target, const base::Target &dependency) {
  tf::Task target_task;
  tf::Task dep_task;
  try {
    target_task = deps_.at(target.GetName());
    dep_task = deps_.at(dependency.GetName());
    target_task.succeed(dep_task);
  } catch (const std::out_of_range &e) {
    env::assert_fatal(false, "Call Register::Build API on target and "
                             "dependency before Register::Dep API");
  }
}

void Register::RunBuild() {
  executor_.run(taskflow_);
  executor_.wait_for_all();
}

void Register::RunTest() {
  for (const auto &t : tests_) {
    env::log_info(__FUNCTION__, fmt::format("Testing \'{}\'", t.first));
    t.second.cb_(t.second.target_);
  }
}

} // namespace buildcc
