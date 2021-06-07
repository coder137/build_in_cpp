#include "target.h"

#include "assert_fatal.h"

#include "internal/util.h"

namespace buildcc::base {

void Target::AddLibDir(const fs::path &relative_lib_dir) {
  env::log_trace(name_, __FUNCTION__);

  fs::path final_lib_dir = target_root_source_dir_ / relative_lib_dir;
  AddLibDirAbsolute(final_lib_dir);
}

void Target::AddLibDirAbsolute(const fs::path &absolute_lib_dir) {
  env::log_trace(name_, __FUNCTION__);

  current_lib_dirs_.insert(internal::quote(
      internal::Path::CreateNewPath(absolute_lib_dir).GetPathname().string()));
}

void Target::AddLibDep(const Target &lib_dep) {
  env::log_trace(name_, __FUNCTION__);

  const fs::path lib_dep_path = lib_dep.GetTargetPath();
  internal::add_path(lib_dep_path, current_lib_deps_);
}

void Target::AddLibDep(const std::string &lib_dep) {
  env::log_trace(name_, __FUNCTION__);
  current_external_lib_deps_.insert(lib_dep);
}

} // namespace buildcc::base
