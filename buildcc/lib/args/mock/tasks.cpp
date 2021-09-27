#include "args/register.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  std::string targetpath = target.GetTargetPath()
                               .lexically_relative(env::get_project_build_dir())
                               .string();
  std::replace(targetpath.begin(), targetpath.end(), '\\', '/');
  mock().actualCall(fmt::format("BuildTask_{}", target.GetName()).c_str());
  return targets_.tf.placeholder().name(targetpath);
}

void Register::RunBuild() {}

} // namespace buildcc
