#include "target.h"

#include "assert_fatal.h"

#include "internal/util.h"

namespace buildcc::base {

void Target::AddLibDep(const Target &lib_dep) {
  env::log_trace(__FUNCTION__, name_);

  const fs::path lib_dep_path = lib_dep.GetTargetPath();
  internal::add_path(lib_dep_path, current_lib_deps_);
}

} // namespace buildcc::base
