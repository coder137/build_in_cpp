#include "target/target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

void Target::CompileTask() {
  ConvertForCompile();

  std::vector<fs::path> compile_sources;
  std::vector<fs::path> dummy_sources;
  BuildCompile(compile_sources, dummy_sources);
  (void)dummy_sources;
  for (const auto &cs : compile_sources) {
    CompileSource(cs);
  }
}

void Target::LinkTask() {
  ConvertForLink();
  BuildLink();
}

} // namespace buildcc::base
