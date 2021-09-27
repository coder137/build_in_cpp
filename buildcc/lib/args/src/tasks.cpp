#include "args/register.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  return deps_.tf_.composed_of(target.GetTaskflow()).name("Task");
}

void Register::RunBuild() {
  executor_.run(deps_.tf_);
  executor_.wait_for_all();
}

} // namespace buildcc
