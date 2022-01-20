#include "target/target.h"

#include "expect_target.h"

namespace buildcc::m {

void TargetRunner(Target &target) {
  tf::Executor executor(1);
  executor.run(target.GetTaskflow());
  executor.wait_for_all();
}

} // namespace buildcc::m
