#include "static_target_gcc.h"

#include "internal/util.h"

// Env
#include "assert_fatal.h"

namespace buildcc {

std::vector<std::string>
StaticTarget_gcc::Link(const std::string &output_target,
                       const std::string &aggregated_link_flags,
                       const std::string &aggregated_compiled_sources,
                       const std::string &aggregated_lib_deps) const {
  (void)aggregated_link_flags;
  (void)aggregated_lib_deps;
  return {
      GetToolchain().GetArchiver(),
      "rcs",
      output_target,
      aggregated_compiled_sources,
  };
}

} // namespace buildcc
