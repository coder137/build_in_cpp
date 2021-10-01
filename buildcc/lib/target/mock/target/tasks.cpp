#include "target/target.h"

namespace buildcc::base {

void Target::CompileTask() {
  ConvertForCompile();

  std::vector<fs::path> source_files;
  std::vector<fs::path> dummy_source_files;

  if (!loader_.IsLoaded()) {
    CompileSources(source_files);
    dirty_ = true;
  } else {
    RecheckFlags(loader_.GetLoadedPreprocessorFlags(),
                 current_preprocessor_flags_);
    RecheckFlags(loader_.GetLoadedCommonCompileFlags(),
                 current_common_compile_flags_);
    RecheckFlags(loader_.GetLoadedAsmCompileFlags(),
                 current_asm_compile_flags_);
    RecheckFlags(loader_.GetLoadedCCompileFlags(), current_c_compile_flags_);
    RecheckFlags(loader_.GetLoadedCppCompileFlags(),
                 current_cpp_compile_flags_);
    RecheckDirs(loader_.GetLoadedIncludeDirs(), current_include_dirs_);
    RecheckPaths(loader_.GetLoadedHeaders(), current_header_files_.internal);
    RecheckPaths(loader_.GetLoadedCompileDependencies(),
                 current_compile_dependencies_.internal);

    if (dirty_) {
      CompileSources(source_files);
    } else {
      RecompileSources(source_files, dummy_source_files);
    }
  }

  for (const auto &s : source_files) {
    bool success = Command::Execute(CompileCommand(s));
    env::assert_fatal(success, "Could not compile source");
  }
}

void Target::LinkTask() {
  ConvertForLink();

  RecheckFlags(loader_.GetLoadedLinkFlags(), current_link_flags_);
  RecheckDirs(loader_.GetLoadedLibDirs(), current_lib_dirs_);
  RecheckExternalLib(loader_.GetLoadedExternalLibDeps(),
                     current_external_lib_deps_);
  RecheckPaths(loader_.GetLoadedLinkDependencies(),
               current_link_dependencies_.internal);
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_.internal);

  if (dirty_) {
    bool success = Command::Execute(LinkCommand());
    env::assert_fatal(success, "Failed to link target");
    Store();
    build_ = true;
  }
}

} // namespace buildcc::base
