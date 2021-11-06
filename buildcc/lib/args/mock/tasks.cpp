#include "args/register.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc {

tf::Task Register::BuildTargetTask(base::Target &target) {
  mock().actualCall(fmt::format("BuildTask_{}", target.GetName()).c_str());
  return build_tf_.placeholder().name(target.GetUniqueId());
}

tf::Task Register::BuildGeneratorTask(base::Generator &generator) {
  mock().actualCall(
      fmt::format("BuildGeneratorTask_{}", generator.GetName()).c_str());
  return build_tf_.placeholder().name(generator.GetUniqueId());
}

void Register::RunBuild() {}

void Register::RunTest() {
  std::for_each(tests_.begin(), tests_.end(),
                [](const auto &p) { p.second.TestRunner(); });
}

} // namespace buildcc
