#include "target/generator.h"

#include "expect_generator.h"

namespace buildcc::m {

void CustomGeneratorRunner(CustomGenerator &custom_generator) {
  tf::Executor executor(1);
  executor.run(custom_generator.GetTaskflow());
  executor.wait_for_all();
}

void GeneratorRunner(Generator &generator) {
  tf::Executor executor(1);
  executor.run(generator.GetTaskflow());
  executor.wait_for_all();
}

} // namespace buildcc::m
