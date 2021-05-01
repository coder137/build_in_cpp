#include "target.h"

#include "internal/util.h"

#include "assert_fatal.h"

#include "fmt/format.h"

namespace buildcc::base {

// * Load
// TODO, Verify things that cannot be changed
// * Compile
// Include directories dependencies
// * Link
// Library dependencies
void Target::Build() {
  env::log_trace(name_, __FUNCTION__);

  // TODO, Optimize these
  aggregated_preprocessor_flags_ =
      internal::aggregate(current_preprocessor_flags_);
  aggregated_c_compile_flags_ = internal::aggregate(current_c_compile_flags_);
  aggregated_cpp_compile_flags_ =
      internal::aggregate(current_cpp_compile_flags_);
  aggregated_link_flags_ = internal::aggregate(current_link_flags_);

  aggregated_lib_deps_ =
      fmt::format("{} {}", internal::aggregate(current_external_lib_deps_),
                  internal::aggregate(current_lib_deps_));

  aggregated_include_dirs_ = internal::aggregate_with_prefix(
      prefix_include_dir_, current_include_dirs_);
  aggregated_lib_dirs_ =
      internal::aggregate_with_prefix(prefix_lib_dir_, current_lib_dirs_);

  const bool is_loaded = loader_.Load();
  if (!is_loaded) {
    BuildCompile();
  } else {
    BuildRecompile();
  }

  dirty_ = false;
}

void Target::BuildCompile() {
  CompileSources();
  BuildTarget();
  Store();
  first_build_ = true;
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
  RecheckDirs(loader_.GetLoadedIncludeDirs(), current_include_dirs_);
  RecheckPaths(loader_.GetLoadedHeaders(), current_header_files_);

  // * Compile sources
  if (dirty_) {
    CompileSources();
  } else {
    RecompileSources();
  }

  // * Completely rebuild target / link if any of the following change
  // Target compiled source files either during Compile / Recompile
  // Target library dependencies
  RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
  RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_);
  if (dirty_) {
    BuildTarget();
    Store();
    rebuild_ = true;
  }
}

void Target::BuildTarget() {
  env::log_trace(name_, __FUNCTION__);

  // Add compiled sources
  const std::string aggregated_compiled_sources =
      internal::aggregate(GetCompiledSources());

  const std::string output_target = internal::quote(GetTargetPath().string());
  bool success = internal::command(
      Link(output_target, aggregated_link_flags_, aggregated_compiled_sources,
           aggregated_lib_dirs_, aggregated_lib_deps_));

  env::assert_fatal(success, fmt::format("Compilation failed for: {}", name_));
}

std::vector<std::string>
Target::Link(const std::string &output_target,
             const std::string &aggregated_link_flags,
             const std::string &aggregated_compiled_sources,
             const std::string &aggregated_lib_dirs,
             const std::string &aggregated_lib_deps) const {
  return {
      // TODO, Let user decide this during Linking phase
      toolchain_.GetCppCompiler(),
      aggregated_link_flags,
      aggregated_compiled_sources,
      "-o",
      output_target,
      aggregated_lib_dirs,
      aggregated_lib_deps,
  };
}

} // namespace buildcc::base
