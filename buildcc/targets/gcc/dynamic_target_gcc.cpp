#include "dynamic_target_gcc.h"

#include "internal/util.h"

// Env
#include "assert_fatal.h"

namespace buildcc {

// Compiling
std::vector<std::string> DynamicTarget_gcc::CompileCommand(
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
      "-fpic",
      "-o",
      output_source,
      "-c",
      input_source,
  };
}

// Linking
std::vector<std::string>
DynamicTarget_gcc::Link(const std::string &output_target,
                        const std::string &aggregated_link_flags,
                        const std::string &aggregated_compiled_sources,
                        const std::string &aggregated_lib_dirs,
                        const std::string &aggregated_lib_deps) const {
  (void)aggregated_lib_dirs;
  (void)aggregated_lib_deps;
  // clang-format off
  return {
      GetToolchain().GetCppCompiler(),
      "-shared",
      aggregated_link_flags,
      aggregated_compiled_sources,
      "-o",
      output_target,
  };
}

} // namespace buildcc
