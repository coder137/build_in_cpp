#ifndef TARGET_INCLUDE_EXECUTABLE_TARGET_H_
#define TARGET_INCLUDE_EXECUTABLE_TARGET_H_

#include "target.h"

namespace buildcc {

class ExecutableTarget : public base::Target {
public:
  ExecutableTarget(const std::string &name, const base::Toolchain &toolchain,
                   const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::Executable, toolchain,
               target_path_relative_to_root) {}
};

} // namespace buildcc

#endif
