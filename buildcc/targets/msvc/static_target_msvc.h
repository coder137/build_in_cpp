#ifndef TARGETS_MSVC_STATIC_TARGET_MSVC_H_
#define TARGETS_MSVC_STATIC_TARGET_MSVC_H_

#include "executable_target_msvc.h"

namespace buildcc {

class StaticTarget_msvc : public base::Target {
public:
  StaticTarget_msvc(const std::string &name, const base::Toolchain &toolchain,
                    const std::filesystem::path &target_path_relative_to_root)
      : Target(name, base::TargetType::StaticLibrary, toolchain,
               target_path_relative_to_root) {
    prefix_include_dir_ = "/I";
  }

private:
  // Compiling
  virtual std::vector<std::string>
  CompileCommand(const std::string &input_source,
                 const std::string &output_source, const std::string &compiler,
                 const std::string &aggregated_preprocessor_flags,
                 const std::string &aggregated_compile_flags,
                 const std::string &aggregated_include_dirs) const override;

  // Linking
  virtual std::vector<std::string>
  Link(const std::string &output_target,
       const std::string &aggregated_link_flags,
       const std::string &aggregated_compiled_sources,
       const std::string &aggregated_lib_dirs,
       const std::string &aggregated_lib_deps) const override;
};

} // namespace buildcc

#endif
