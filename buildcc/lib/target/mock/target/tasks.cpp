#include "target/target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

void Target::CompileTask() {
  BuildCompileGenerator();
  compile_generator_.Build();
}

void Target::LinkTask() {
  BuildLinkGenerator();
  link_generator_.Build();
}

} // namespace buildcc::base
