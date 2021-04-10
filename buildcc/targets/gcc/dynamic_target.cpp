#include "dynamic_target.h"

#include "internal/util.h"

// Env
#include "assert_fatal.h"

namespace buildcc {

// Compiling
void DynamicTarget::CompileSource(const fs::path &current_source,
                                  const std::string &aggregated_include_dirs) {
  const std::string compiled_source = GetCompiledSourceName(current_source);
  const std::string compiler = GetCompiler(current_source);

  bool success = internal::command({
      compiler,
      // TODO, Add Preprocessor Flags
      aggregated_include_dirs,
      "-fpic",
      // TODO, Add C/Cpp Compile Flags
      "-o",
      compiled_source,
      "-c",
      current_source.string(),
  });
  env::assert_fatal(success,
                    "Compilation failed for: " + current_source.string());
}

// Linking
std::vector<std::string>
DynamicTarget::Link(const std::string &output_target,
                    const std::string &aggregated_link_flags,
                    const std::string &aggregated_compiled_sources,
                    const std::string &aggregated_lib_deps) {
  return {
      GetToolchain().GetDynamicLibCompiler(),
      "-shared",
      aggregated_compiled_sources,
      "-o",
      output_target,
  };
}

} // namespace buildcc
