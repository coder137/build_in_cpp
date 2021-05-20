#ifndef BUILDCC_INCLUDE_ARGS_H_
#define BUILDCC_INCLUDE_ARGS_H_

#include "CLI/CLI.hpp"

namespace buildcc {

class Args {
public:
  struct Toolchain {
    bool build;
    bool test;
  };

public:
  Args() { Initialize(); }
  Args(const Args &) = delete;

  void Parse(int argc, char **argv);
  CLI::App &Ref() { return app_; }
  const CLI::App &ConstRef() const { return app_; }

  // Setters
  void AddCustomToolchain(const std::string &name,
                          const std::string &description,
                          Toolchain &custom_toolchain_arg);

  // Getters
  bool Clean() { return clean_; }

  const Toolchain &GccToolchain() { return gcc_toolchain_; }
  const Toolchain &MsvcToolchain() { return msvc_toolchain_; }

private:
  void Initialize();

  void RootArgs();
  void ToolchainArgs();
  void AddToolchain(CLI::App *toolchain, Toolchain &arg_toolchain);

private:
  CLI::App app_{"BuildCC buildsystem"};

  bool clean_{false};

  CLI::App *toolchain_{nullptr};
  Toolchain gcc_toolchain_{false, false};
  Toolchain msvc_toolchain_{false, false};
};

} // namespace buildcc

#endif
