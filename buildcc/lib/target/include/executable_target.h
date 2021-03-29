#ifndef TARGET_INCLUDE_EXECUTABLE_TARGET_H_
#define TARGET_INCLUDE_EXECUTABLE_TARGET_H_

#include "base/target.h"

namespace buildcc {

class ExecutableTarget : public Target {
public:
  ExecutableTarget(const std::string &name, const Toolchain &toolchain,
                   const std::filesystem::path &target_path_relative_to_root)
      : Target(name, TargetType::Executable, toolchain,
               target_path_relative_to_root) {}
};

} // namespace buildcc

#endif
