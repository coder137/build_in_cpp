#include "args/args.h"

#include "env/assert_fatal.h"

namespace buildcc {

void Args::Parse(int argc, const char *const *argv) {
  try {
    app_.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    env::assert_fatal<false>(e.what());
  }
}

} // namespace buildcc
