#include "target.h"

namespace buildcc::base {

void Target::AddPreprocessorFlag(const std::string &flag) {
  current_preprocessor_flags_.insert(flag);
}
void Target::AddCCompileFlag(const std::string &flag) {
  current_c_compile_flags_.insert(flag);
}
void Target::AddCppCompileFlag(const std::string &flag) {
  current_cpp_compile_flags_.insert(flag);
}
void Target::AddLinkFlag(const std::string &flag) {
  current_link_flags_.insert(flag);
}

} // namespace buildcc::base
