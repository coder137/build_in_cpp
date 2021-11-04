#include "args/register.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  mock().actualCall(fmt::format("BuildTask_{}", target.GetName()).c_str());
  return build_tf_.placeholder().name(target.GetUniqueId());
}

void Register::RunBuild() {}

} // namespace buildcc
