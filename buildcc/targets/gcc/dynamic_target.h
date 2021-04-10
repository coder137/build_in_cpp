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
  virtual std::vector<std::string>
  CompileCommand(const std::string &input_source,
                 const std::string &output_source, const std::string &compiler,
                 const std::string &aggregated_preprocessor_flags,
                 const std::string &aggregated_compile_flags,
                 const std::string &aggregated_include_dirs) override;

  // Linking
  virtual std::vector<std::string>
  Link(const std::string &output_target,
       const std::string &aggregated_link_flags,
       const std::string &aggregated_compiled_sources,
       const std::string &aggregated_lib_deps) override;
};

} // namespace buildcc

#endif
