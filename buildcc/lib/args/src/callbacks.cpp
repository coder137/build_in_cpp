#include "args/register.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  return taskflow_.composed_of(target.GetTaskflow()).name("Task");
}

} // namespace buildcc
