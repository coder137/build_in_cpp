#ifndef BUILDCC_INCLUDE_ARGS_H_
#define BUILDCC_INCLUDE_ARGS_H_

#include <filesystem>

#include "logging.h"

#include "CLI/CLI.hpp"

namespace fs = std::filesystem;

namespace buildcc {

class Args {
public:
  struct Toolchain {
    bool build = false;
    bool test = false;
  };

public:
  Args() { Initialize(); }
  Args(const Args &) = delete;

  void Parse(int argc, char **argv);

  // TODO, Check if these are necessary
  CLI::App &Ref() { return app_; }
  const CLI::App &ConstRef() const { return app_; }

  // Setters
  void AddCustomToolchain(const std::string &name,
                          const std::string &description,
                          Toolchain &custom_toolchain_arg);

  // Getters
  bool Clean() const { return clean_; }
  env::LogLevel GetLogLevel() const { return loglevel_; }

  const fs::path &GetProjectRootDir() const { return project_root_dir_; }
  const fs::path &GetProjectBuildDir() const { return project_build_dir_; }
  const Toolchain &GetGccToolchain() const { return gcc_toolchain_; }
  const Toolchain &GetMsvcToolchain() const { return msvc_toolchain_; }

private:
  void Initialize();

  void RootArgs();
  void ToolchainArgs();
  void AddToolchain(const std::string &name, const std::string &description,
                    const std::string &group, Toolchain &toolchain_arg);

private:
  CLI::App app_{"BuildCC buildsystem"};

  bool clean_{false};
  env::LogLevel loglevel_{env::LogLevel::Info};
  const std::map<std::string, env::LogLevel> loglevel_map_{
      {"Trace", env::LogLevel::Trace},
      {"Debug", env::LogLevel::Debug},
      {"Info", env::LogLevel::Info},
      {"Warning", env::LogLevel::Warning},
      {"Critical", env::LogLevel::Critical},
  };

  // directory
  fs::path project_root_dir_{""};
  fs::path project_build_dir_{"_internal"};

  // toolchain
  Toolchain gcc_toolchain_{false, false};
  Toolchain msvc_toolchain_{false, false};

  // Internal
  CLI::App *toolchain_{nullptr};
};

} // namespace buildcc

#endif
