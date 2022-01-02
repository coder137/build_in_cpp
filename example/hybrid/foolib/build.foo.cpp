#include "build.foo.h"

void fooTarget(buildcc::BaseTarget &target, const fs::path &relative_path) {
  target.AddSource(relative_path / "src/foo.cpp");
  target.AddIncludeDir(relative_path / "src", true);
}
