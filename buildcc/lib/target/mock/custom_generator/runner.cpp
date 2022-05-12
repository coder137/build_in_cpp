#include "target/custom_generator.h"

#include "expect_custom_generator.h"

namespace buildcc::m {

void CustomGeneratorRunner(CustomGenerator &custom_generator) {
  tf::Executor executor(1);
  executor.run(custom_generator.GetTaskflow());
  executor.wait_for_all();
}

} // namespace buildcc::m
