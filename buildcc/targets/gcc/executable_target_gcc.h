#ifndef TARGETS_GCC_EXECUTABLE_TARGET_GCC_H_
#define TARGETS_GCC_EXECUTABLE_TARGET_GCC_H_

#include "target.h"

namespace buildcc {

class ExecutableTarget_gcc : public base::Target {
public:
  ExecutableTarget_gcc(
      const std::string &name, const base::Toolchain &toolchain,
      const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {}
};

} // namespace buildcc

#endif
