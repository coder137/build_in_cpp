#include "dynamic_target.h"

#include "internal/assert_fatal.h"
#include "internal/util.h"

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
  buildcc::internal::assert_fatal_true(success, "Compilation failed for: " +
                                                    current_source.string());
}

// Linking
void DynamicTarget::BuildTarget(
    const std::vector<std::string> &compiled_sources) {
  env::log_trace(__FUNCTION__, GetName());

  // Add compiled sources
  std::string aggregated_compiled_sources =
      internal::aggregate_compiled_sources(compiled_sources);

  // TODO, Add compiled libs

  // Final Target
  const fs::path target = GetTargetPath();
  bool success = internal::command({
      // TODO, Improve this logic
      // Select cpp compiler for building target only if there is .cpp file
      // added
      // Else use c compiler
      GetToolchain().GetCppCompiler(),
      "-shared",
      // TODO, Add Link Flags
      aggregated_compiled_sources,
      "-o",
      target.string(),
  });
  // TODO, Library dependencies come after

  internal::assert_fatal_true(success, "Compilation failed for: " + GetName());
}

} // namespace buildcc
