#include "target/target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

void Target::CompileTargetTask(std::vector<fs::path> &&compile_sources,
                               std::vector<fs::path> &&dummy_compile_sources) {
  (void)dummy_compile_sources;
  for (const auto &cs : compile_sources) {
    CompileSource(cs);
  }
}

void Target::LinkTask() { BuildLink(); }

} // namespace buildcc::base
