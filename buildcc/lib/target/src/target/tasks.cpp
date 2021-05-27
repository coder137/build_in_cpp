#include "target.h"

#include "assert_fatal.h"
#include "logging.h"

#include "internal/util.h"

#include "fmt/format.h"

namespace buildcc::base {

void Target::CompileTargetTask(
    const std::vector<fs::path> &&compile_sources,
    const std::vector<fs::path> &&dummy_compile_sources) {
  compile_task_ =
      tf_.emplace([this, compile_sources,
                   dummy_compile_sources](tf::Subflow &subflow) {
           for (const auto &cs : compile_sources) {
             std::string name =
                 cs.lexically_relative(env::get_project_root_dir()).string();
             (void)subflow.emplace([this, cs]() { CompileSource(cs); })
                 .name(name);
           }

           // NOTE, This has just been added for graph generation
           for (const auto &dcs : dummy_compile_sources) {
             std::string name =
                 dcs.lexically_relative(env::get_project_root_dir()).string();
             (void)subflow.emplace([]() {}).name(name);
           }
         })
          .name(kCompileTaskName);
}

void Target::LinkTargetTask() {
  env::log_trace(name_, __FUNCTION__);
  link_task_ = tf_.emplace([this]() { LinkTarget(); }).name(kLinkTaskName);
  link_task_.succeed(compile_task_);
}

} // namespace buildcc::base
