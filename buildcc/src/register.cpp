#include "register.h"

#include <filesystem>

#include "env.h"

namespace fs = std::filesystem;

namespace buildcc {

void Register::Env() {
  env::init(fs::current_path() / args_.GetProjectRootDir(),
            fs::current_path() / args_.GetProjectBuildDir());
  env::set_log_level(args_.GetLogLevel());
}

void Register::Clean(std::function<void(void)> clean_cb) {
  if (args_.Clean()) {
    clean_cb();
  }
}

} // namespace buildcc
