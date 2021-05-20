#include "args.h"

namespace buildcc {

void Args::AddCustomToolchain(const std::string &name,
                              const std::string &description,
                              Toolchain &custom_toolchain_arg) {
  CLI::App *custom_arg = toolchain_->add_subcommand(name, description);
  AddToolchain(custom_arg, custom_toolchain_arg);
}

void Args::Parse(int argc, char **argv) {
  try {
    app_.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    exit(app_.exit(e));
  }
}

// Private

void Args::Initialize() {
  RootArgs();
  ToolchainArgs();
}

void Args::RootArgs() {
  app_.set_help_all_flag("--help-all", "");
  app_.add_flag("-c,--clean", clean_, "");
  app_.set_config("--config", "", "")->expected(1, 3);
}

void Args::ToolchainArgs() {
  toolchain_ = app_.add_subcommand("toolchain", "Select Toolchain")
                   ->required()
                   ->require_subcommand();
  CLI::App *gcc_arg = toolchain_->add_subcommand("gcc", "GNU GCC Toolchain");
  CLI::App *msvc_arg = toolchain_->add_subcommand("msvc", "MSVC Toolchain");

  AddToolchain(gcc_arg, gcc_toolchain_);
  AddToolchain(msvc_arg, msvc_toolchain_);
}

void Args::AddToolchain(CLI::App *toolchain, Toolchain &toolchain_arg) {
  toolchain->add_flag("-b,--build", toolchain_arg.build);
  toolchain->add_flag("-t,--test", toolchain_arg.test);
}

} // namespace buildcc
