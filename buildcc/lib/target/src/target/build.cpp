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
// TODO, Toolchain preprocessor flags
// TODO, Toolchain compile flags
// TODO, Toolchain link flags
// TODO, Target preprocessor flags
// TODO, Target compile flags
// TODO, Target link flags
// Target source files
// Target include dirs
// Target library dependencies
// TODO, Target library directories
void Target::BuildRecompile() {

  // * Completely compile sources if any of the following change
  // TODO, Toolchain, ASM, C, C++ compiler
  // TODO, Toolchain preprocessor flags
  // TODO, Toolchain compile flags
  // TODO, Target preprocessor flags
  // TODO, Target compile flags
  // Target include dirs
  RecheckIncludeDirs();

  // * Compile sources
  std::vector<std::string> compiled_sources;
  if (dirty_) {
    compiled_sources = CompileSources();
  } else {
    compiled_sources = RecompileSources();
  }

  // * Completely rebuild target / link if any of the following change
  // TODO, Toolchain link flags
  // TODO, Target link flags
  // Target compiled source files either during Compile / Recompile
  // Target library dependencies
  // TODO, Target library directories
  RecheckLibDeps();
  if (dirty_) {
    BuildTarget(compiled_sources);
    Store();
  }
}

void Target::BuildTarget(const std::vector<std::string> &compiled_sources) {
  env::log_trace(__FUNCTION__, name_);

  // Add compiled sources
  const std::string aggregated_link_flags =
      internal::aggregate_link_flags(link_flags_);
  const std::string aggregated_compiled_sources =
      internal::aggregate_compiled_sources(compiled_sources);

  const std::string aggregated_lib_deps =
      internal::aggregate_lib_deps(current_lib_deps_);

  // Final Target
  // TODO, Improve this logic
  // Select cpp compiler for building target only if there is .cpp file
  // added
  // Else use c compiler
  const fs::path target = GetTargetPath();

  bool success =
      internal::command(Link(target.string(), aggregated_link_flags,
                             aggregated_compiled_sources, aggregated_lib_deps));

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
