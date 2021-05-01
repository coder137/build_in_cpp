#ifndef TARGETS_GCC_STATIC_TARGET_GCC_H_
#define TARGETS_GCC_STATIC_TARGET_GCC_H_

#include "target.h"

namespace buildcc {

class StaticTarget_gcc : public base::Target {
public:
  StaticTarget_gcc(const std::string &name, const base::Toolchain &toolchain,
                   const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root) {}

private:
  // NOTE: Compiling does not need to change

  virtual std::vector<std::string>
  Link(const std::string &output_target,
       const std::string &aggregated_link_flags,
       const std::string &aggregated_compiled_sources,
       const std::string &aggregated_lib_dirs,
       const std::string &aggregated_lib_deps) const override;
};

} // namespace buildcc

#endif
