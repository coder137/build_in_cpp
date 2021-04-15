#include "target.h"

#include "assert_fatal.h"

namespace buildcc::base {

void Target::AddLibDep(const Target &lib_dep) {
  env::log_trace(__FUNCTION__, name_);

  const fs::path lib_dep_path = lib_dep.GetTargetPath();
  env::assert_fatal(fs::exists(lib_dep_path),
                    lib_dep_path.string() + " not found");

  const auto lib_dep_file = internal::Path::CreateExistingPath(lib_dep_path);
  env::assert_fatal(current_lib_deps_.find(lib_dep_file) ==
                        current_lib_deps_.end(),
                    lib_dep_path.string() + " duplicate found");
  current_lib_deps_.insert(lib_dep_file);
}

} // namespace buildcc::base
