#include "static_target.h"

#include "internal/assert_fatal.h"
#include "internal/util.h"

namespace buildcc {

void StaticTarget::BuildTarget(
    const std::vector<std::string> &compiled_sources) {
  env::log_trace(__FUNCTION__, GetName());

  // Add compiled sources
  std::string aggregated_compiled_sources =
      internal::aggregate_compiled_sources(compiled_sources);

  try {
    const std::string &ar = GetToolchain().GetExecutable("ar");
    // Final Target
    const fs::path target = GetTargetIntermediate() / GetName();
    bool success = internal::command({
        ar,
        "rcs",
        target.string(),
        aggregated_compiled_sources,
    });
    internal::assert_fatal_true(success,
                                "Compilation failed for: " + GetName());
  } catch (std::out_of_range) {
    internal::assert_fatal_true(false, "Could not find 'ar' executable");
  }
}

} // namespace buildcc
