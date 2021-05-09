#include "dynamic_target_msvc.h"

#include "fmt/format.h"

namespace buildcc {

std::vector<std::string> DynamicTarget_msvc::CompileCommand(
    const std::string &input_source, const std::string &output_source,
    const std::string &compiler,
    const std::string &aggregated_preprocessor_flags,
    const std::string &aggregated_compile_flags,
    const std::string &aggregated_include_dirs) const {
  return {
      compiler,
      aggregated_preprocessor_flags,
      aggregated_include_dirs,
      aggregated_compile_flags,
      fmt::format("/Fo{}", output_source),
      "/c",
      input_source,
  };
}

// Linking
std::vector<std::string>
DynamicTarget_msvc::Link(const std::string &output_target,
                         const std::string &aggregated_link_flags,
                         const std::string &aggregated_compiled_sources,
                         const std::string &aggregated_lib_dirs,
                         const std::string &aggregated_lib_deps) const {
  (void)aggregated_lib_dirs;
  (void)aggregated_lib_deps;
  return {
      GetToolchain().GetLinker(),
      "/DLL",
      aggregated_link_flags,
      fmt::format("/OUT:{}.dll", output_target),
      fmt::format("/IMPLIB:{}", output_target),
      aggregated_compiled_sources,
  };
}

} // namespace buildcc
