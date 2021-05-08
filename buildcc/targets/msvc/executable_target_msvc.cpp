#include "executable_target_msvc.h"

#include "internal/util.h"

// Env
#include "assert_fatal.h"

namespace buildcc {

// Compiling
std::vector<std::string> ExecutableTarget_msvc::CompileCommand(
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
      "/Fo" + output_source,
      "/c",
      input_source,
  };
}

// Linking
std::vector<std::string>
ExecutableTarget_msvc::Link(const std::string &output_target,
                            const std::string &aggregated_link_flags,
                            const std::string &aggregated_compiled_sources,
                            const std::string &aggregated_lib_dirs,
                            const std::string &aggregated_lib_deps) const {
  // clang-format off
  return {
      GetToolchain().GetLinker(),
      aggregated_link_flags,
      aggregated_lib_dirs,
      "/OUT:" + output_target,
      aggregated_lib_deps,
      aggregated_compiled_sources,
  };
  // clang-format on
}

} // namespace buildcc
