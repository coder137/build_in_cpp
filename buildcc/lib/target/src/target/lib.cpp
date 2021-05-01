#include "target.h"

#include "assert_fatal.h"

#include "internal/util.h"

namespace buildcc::base {

void Target::AddLibDir(const fs::path &absolute_lib_dir) {
  fs::path final_lib_dir = absolute_lib_dir;
  current_lib_dirs_.insert(final_lib_dir.make_preferred().string());
}

void Target::AddLibDep(const Target &lib_dep) {
  env::log_trace(name_, __FUNCTION__);

  const fs::path lib_dep_path = lib_dep.GetTargetPath();
  internal::add_path(lib_dep_path, current_lib_deps_);
}

} // namespace buildcc::base
