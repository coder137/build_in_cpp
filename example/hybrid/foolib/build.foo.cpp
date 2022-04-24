#include "build.foo.h"

void fooTarget(buildcc::TargetInfo &target) {
  target.AddSource("src/foo.cpp");
  target.AddIncludeDir("src", true);
}
