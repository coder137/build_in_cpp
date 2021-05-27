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

void Register::BuildTarget(bool build, base::Target &target,
                           std::function<void(base::Target &)> build_cb) {
  if (build) {
    taskflow_.composed_of(target.GetTaskflow()).name("Task");
    build_cb(target);
  }
}
void Register::TestTarget(bool test, base::Target &target,
                          std::function<void(base::Target &)> test_cb) {
  if (!test) {
    return;
  }

  const bool added =
      tests_.emplace(target.GetName(), TestInfo(target, test_cb)).second;
  env::assert_fatal(
      added, fmt::format("Could not register test {}", target.GetName()));
}

void Register::BuildAll() {
  executor_.run(taskflow_);
  executor_.wait_for_all();
}

void Register::TestAll() {
  for (const auto &t : tests_) {
    env::log_info(__FUNCTION__, fmt::format("Testing \'{}\'", t.first));
    t.second.cb_(t.second.target_);
  }
}

} // namespace buildcc
