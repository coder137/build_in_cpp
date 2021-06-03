#ifndef PLUGINS_CLANG_COMPILE_COMMANDS_H_
#define PLUGINS_CLANG_COMPILE_COMMANDS_H_

#include "target.h"

namespace buildcc::plugin {

class ClangCompileCommands {
public:
  explicit ClangCompileCommands(std::vector<const base::Target *> targets)
      : targets_(std::move(targets)) {}
  ClangCompileCommands(const ClangCompileCommands &compile_commands) = delete;

  void AddTarget(const base::Target *target);

  void Generate();

private:
  std::vector<const base::Target *> targets_;
};

} // namespace buildcc::plugin

#endif
