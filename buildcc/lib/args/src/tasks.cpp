#include "args/register.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  std::string targetpath = target.GetTargetPath()
                               .lexically_relative(env::get_project_build_dir())
                               .string();
  std::replace(targetpath.begin(), targetpath.end(), '\\', '/');
  return targets_.tf.composed_of(target.GetTaskflow()).name(targetpath);
}

void Register::RunBuild() {
  executor_.run(targets_.tf);
  executor_.wait_for_all();
}

} // namespace buildcc
