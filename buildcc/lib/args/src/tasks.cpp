#include "args/register.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  return targets_.tf.composed_of(target.GetTaskflow()).name("Task");
}

void Register::RunBuild() {
  executor_.run(targets_.tf);
  executor_.wait_for_all();
}

} // namespace buildcc
