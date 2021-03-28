#ifndef TARGET_INCLUDE_STATIC_TARGET_H_
#define TARGET_INCLUDE_STATIC_TARGET_H_

#include "target.h"

namespace buildcc {

class StaticTarget : public Target {
  StaticTarget(const std::string &name, TargetType type,
               const Toolchain &toolchain,
               const std::filesystem::path &target_path_relative_to_root)
      : Target(name, type, toolchain, target_path_relative_to_root) {}

private:
  // NOTE: Compiling does not need to change

  // Linking needs to change
  void BuildTarget(const std::vector<std::string> &compiled_sources) override;
};

} // namespace buildcc

#endif
