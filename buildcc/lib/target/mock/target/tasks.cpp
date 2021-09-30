#include "target/target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

void Target::CompileTask() { compile_generator_.Build(); }

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
  }

  build_ = true;
}

} // namespace buildcc::base
