#include "toolchain.h"

namespace buildcc {

bool Toolchain::AddExecutable(std::string name, std::string executable) {
  if (executables_.find(name) == executables_.end()) {
    executables_[name] = executable;
    return true;
  }
  return false;
}

// TODO, Add other setter definitions here

} // namespace buildcc
