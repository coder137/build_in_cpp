#include "target/target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

void Target::CompileTask() { compile_generator_.Build(); }

void Target::LinkTask() { link_generator_.Build(); }

} // namespace buildcc::base
