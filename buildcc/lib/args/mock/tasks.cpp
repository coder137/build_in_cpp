#include "args/register.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  mock().actualCall(fmt::format("BuildTask_{}", target.GetName()).c_str());
  return deps_.tf_.placeholder();
}

void Register::RunBuild() {}

} // namespace buildcc
