#ifndef TARGET_INCLUDE_STATIC_TARGET_H_
#define TARGET_INCLUDE_STATIC_TARGET_H_

#include "base/target.h"

namespace buildcc {

class StaticTarget : public Target {
public:
  StaticTarget(const std::string &name, const Toolchain &toolchain,
               const std::filesystem::path &target_path_relative_to_root)
      : Target(name, TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root) {}

private:
  // NOTE: Compiling does not need to change

  // Linking needs to change
  void BuildTarget(const std::vector<std::string> &compiled_sources) override;
};

} // namespace buildcc

#endif
