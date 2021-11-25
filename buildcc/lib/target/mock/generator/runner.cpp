#include "target/generator.h"

#include "expect_generator.h"

namespace buildcc::base::m {

void GeneratorRunner(Generator &generator) {
  tf::Executor executor(1);
  executor.run(generator.GetTaskflow());
  executor.wait_for_all();
}

} // namespace buildcc::base::m
