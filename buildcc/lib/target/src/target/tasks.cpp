/*
 * Copyright 2021 Niket Naidu. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "target/target.h"

#include <algorithm>

#include "env/assert_fatal.h"
#include "env/logging.h"

#include "target/util.h"

#include "fmt/format.h"

namespace {

constexpr const char *const kCompileTaskName = "Compile";
constexpr const char *const kLinkTaskName = "Link";

} // namespace

namespace buildcc::base {

void Target::CompileTargetTask(std::vector<fs::path> &&compile_sources,
                               std::vector<fs::path> &&dummy_compile_sources) {
  env::log_trace(name_, __FUNCTION__);

  compile_task_ = tf_.emplace(
      [this, compile_sources, dummy_compile_sources](tf::Subflow &subflow) {
        for (const auto &cs : compile_sources) {
          std::string name =
              cs.lexically_relative(env::get_project_root_dir()).string();
          (void)subflow.emplace([this, cs]() { CompileSource(cs); }).name(name);
        }

        // NOTE, This has just been added for graph generation
        for (const auto &dcs : dummy_compile_sources) {
          std::string name =
              dcs.lexically_relative(env::get_project_root_dir()).string();
          (void)subflow.emplace([]() {}).name(name);
        }
      });
  compile_task_.name(kCompileTaskName);
}

void Target::LinkTargetTask(const bool link) {
  env::log_trace(name_, __FUNCTION__);

  link_task_ = tf_.emplace([this, link]() {
    if (link) {
      dirty_ = true;
    }

    std::for_each(target_lib_deps_.cbegin(), target_lib_deps_.cend(),
                  [this](const Target *target) {
                    current_lib_deps_.insert(internal::Path::CreateExistingPath(
                        target->GetTargetPath()));
                  });
    RecheckPaths(loader_.GetLoadedLibDeps(), current_lib_deps_);
    if (dirty_ || link) {
      LinkTarget();
      Store();
    }
  });

  link_task_.name(kLinkTaskName);
  link_task_.succeed(compile_task_);
}

} // namespace buildcc::base
