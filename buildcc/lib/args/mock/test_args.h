#ifndef ARGS_MOCK_TEST_ARGS_H_
#define ARGS_MOCK_TEST_ARGS_H_

#include "args/args.h"

#include <cassert>

namespace buildcc::m {

inline void ArgsDeinit() {
  auto subcommand_list =
      buildcc::Args::Ref().get_subcommands([](CLI::App *) { return true; });
  for (auto *subcommand : subcommand_list) {
    assert(buildcc::Args::Ref().remove_subcommand(subcommand));
  }
  buildcc::Args::Ref().clear();
}

} // namespace buildcc::m

#endif
