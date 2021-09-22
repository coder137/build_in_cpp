#include "args/args.h"

namespace buildcc {

void Args::Parse(int argc, const char *const *argv) {
  try {
    app_.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    exit(app_.exit(e));
  }
}

} // namespace buildcc
