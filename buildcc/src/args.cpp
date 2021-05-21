#include "args.h"

namespace buildcc {

void Args::AddCustomToolchain(const std::string &name,
                              const std::string &description,
                              Toolchain &custom_toolchain_arg) {
  AddToolchain(name, description, "Custom", custom_toolchain_arg);
}

void Args::Parse(int argc, char **argv) {
  try {
    app_.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    env::log_critical("Args::Parse", e.what());
    exit(app_.exit(e));
  }
}

// Private

void Args::Initialize() {
  RootArgs();
  ToolchainArgs();
}

void Args::RootArgs() {
  app_.set_help_all_flag("--help-all", "Expand individual options");

  // TODO, Currently only expects 1
  // From CLI11 2.0 onwards multiple configuration files can be added, increase
  // this limit
  app_.set_config("--config", "", "Read a <config>.toml file")->expected(1);

  // Root flags
  app_.add_flag("-c,--clean", clean_, "Clean artifacts")->group("Root");
  app_.add_option("-l,--loglevel", loglevel_, "LogLevel settings")
      ->transform(CLI::CheckedTransformer(loglevel_map_, CLI::ignore_case))
      ->group("Root");

  // Dir flags
  app_.add_option("--root_dir", project_root_dir_,
                  "Project root directory (relative to current directory)")
      ->required()
      ->group("Root");
  app_.add_option("--build_dir", project_build_dir_,
                  "Project build dir (relative to current directory)")
      ->required()
      ->group("Root");
}

void Args::ToolchainArgs() {
  toolchain_ = app_.add_subcommand("toolchain", "Select Toolchain")
                   ->required()
                   ->require_subcommand();
  AddToolchain("gcc", "GNU GCC Toolchain", "Supported", gcc_toolchain_);
  AddToolchain("msvc", "MSVC Toolchain", "Supported", msvc_toolchain_);
}

void Args::AddToolchain(const std::string &name, const std::string &description,
                        const std::string &group, Toolchain &toolchain_arg) {
  CLI::App *toolchain_spec =
      toolchain_->add_subcommand(name, description)->group(group);
  toolchain_spec->add_flag("-b,--build", toolchain_arg.build);
  toolchain_spec->add_flag("-t,--test", toolchain_arg.test);
}

} // namespace buildcc
