#include "target.h"

#include "internal/util.h"

#include "assert_fatal.h"

namespace buildcc::base {

// * Load
// TODO, Verify things that cannot be changed
// * Compile
// Include directories dependencies
// * Link
// Library dependencies
void Target::Build() {
  env::log_trace(__FUNCTION__, name_);

  aggregated_preprocessor_flags_ =
      internal::aggregate(current_preprocessor_flags_);
  aggregated_c_compile_flags_ = internal::aggregate(current_c_compile_flags_);
  aggregated_cpp_compile_flags_ =
      internal::aggregate(current_cpp_compile_flags_);
  aggregated_link_flags_ = internal::aggregate(current_link_flags_);

  aggregated_include_dirs_ =
      internal::aggregate_include_dirs(current_include_dirs_);
  aggregated_lib_deps_ = internal::aggregate(current_lib_deps_);

  const bool is_loaded = loader_.Load();
  if (!is_loaded) {
    BuildCompile();
  } else {
    BuildRecompile();
  }

  dirty_ = false;
}

void Target::BuildCompile() {
  const std::vector<std::string> compiled_sources = CompileSources();
  BuildTarget(compiled_sources);
  Store();
}

// * Target rebuild depends on
// TODO, Toolchain name
// DONE, Target preprocessor flags
// DONE, Target compile flags
// DONE, Target link flags
// DONE, Target source files
// DONE, Target include dirs
// DONE, Target library dependencies
// TODO, Target library directories
void Target::BuildRecompile() {

  // * Completely compile sources if any of the following change
  // TODO, Toolchain, ASM, C, C++ compiler related to a particular name
  RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
               current_preprocessor_flags_);
  RecheckFlags(loader_.GetLoadedCCompileFlags(), current_c_compile_flags_);
  RecheckFlags(loader_.GetLoadedCppCompileFlags(), current_cpp_compile_flags_);
  RecheckPaths(loader_.GetLoadedIncludeDirs(), current_include_dirs_);

  // * Compile sources
  std::vector<std::string> compiled_sources;
  if (dirty_) {
    compiled_sources = CompileSources();
  } else {
    compiled_sources = RecompileSources();
  }

  // * Completely rebuild target / link if any of the following change
  // Target compiled source files either during Compile / Recompile
  // Target library dependencies
  // TODO, Target library directories
  RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_);
  if (dirty_) {
    BuildTarget(compiled_sources);
    Store();
  }
}

void Target::BuildTarget(const std::vector<std::string> &compiled_sources) {
  env::log_trace(__FUNCTION__, name_);

  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(compiled_sources);

  // Final Target
  // TODO, Improve this logic
  // Select cpp compiler for building target only if there is .cpp file
  // added
  // Else use c compiler
  const fs::path target = GetTargetPath();

  bool success = internal::command(Link(target.string(), aggregated_link_flags_,
                                        aggregated_compiled_sources,
                                        aggregated_lib_deps_));

  env::assert_fatal(success, "Compilation failed for: " + GetName());
}

std::vector<std::string>
Target::Link(const std::string &output_target,
             const std::string &aggregated_link_flags,
             const std::string &aggregated_compiled_sources,
             const std::string &aggregated_lib_deps) {
  return {
      toolchain_.GetCppCompiler(),
      aggregated_link_flags,
      aggregated_compiled_sources,
      "-o",
      output_target,
      aggregated_lib_deps,
  };
}

} // namespace buildcc::base
