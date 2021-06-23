#include "target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

void Target::CompileTargetTask(
    const std::vector<fs::path> &&compile_sources,
    const std::vector<fs::path> &&dummy_compile_sources) {
  (void)dummy_compile_sources;
  for (const auto &cs : compile_sources) {
    CompileSource(cs);
  }
}

void Target::LinkTargetTask(const bool link) {
  if (link) {
    dirty_ = true;
  }
  std::for_each(target_lib_deps_.cbegin(), target_lib_deps_.cend(),
                [this](const Target *target) {
                  current_lib_deps_.insert(internal::Path::CreateExistingPath(
                      target->GetTargetPath()));
                });
  RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_);
  if (dirty_) {
    LinkTarget();
    Store();
  }
}

} // namespace buildcc::base
