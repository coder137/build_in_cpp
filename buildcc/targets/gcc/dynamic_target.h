#ifndef TARGET_INCLUDE_DYNAMIC_TARGET_H_
#define TARGET_INCLUDE_DYNAMIC_TARGET_H_

#include "target.h"

namespace buildcc {

class DynamicTarget : public base::Target {
public:
  DynamicTarget(const std::string &name, const base::Toolchain &toolchain,
                const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::DynamicLibrary, toolchain,
               target_path_relative_to_root) {}

private:
  // Compiling
  void CompileSource(const fs::path &current_source,
                     const std::string &aggregated_include_dirs) override;

  // Linking
  void BuildTarget(const std::vector<std::string> &compiled_sources) override;
};

} // namespace buildcc

#endif
