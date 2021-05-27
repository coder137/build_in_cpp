#include "target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

void Target::CompileTargetTask(
    const std::vector<fs::path> &&compile_sources,
    const std::vector<fs::path> &&dummy_compile_sources) {
  (void)dummy_compile_sources;
  for (const auto &cs : compile_sources) {
    CompileSource(cs);
  }
}

void Target::LinkTargetTask(const bool link) {
  if (link) {
    LinkTarget();
  }
}

} // namespace buildcc::base
