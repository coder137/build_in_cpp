/*
 * Copyright 2021-2022 Niket Naidu. All rights reserved.
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

#ifndef TARGET_TARGET_INFO_H_
#define TARGET_TARGET_INFO_H_

#include <string>

#include "toolchain/toolchain.h"

#include "target/common/target_config.h"
#include "target/common/target_env.h"

#include "target/api/deps_api.h"
#include "target/api/include_api.h"
#include "target/api/lib_api.h"
#include "target/api/pch_api.h"
#include "target/api/source_api.h"
#include "target/api/sync_api.h"
#include "target/api/target_info_getter.h"

#include "schema/target_serialization.h"

namespace buildcc {

// TODO, Make this private
struct UserTargetSchema : public internal::TargetSchema {
  fs_unordered_set sources;
  fs_unordered_set headers;
  fs_unordered_set pchs;

  std::vector<fs::path> libs;

  fs_unordered_set compile_dependencies;
  fs_unordered_set link_dependencies;
};

// NOTE: BaseTarget info is meant to hold information that is common to
// multiple targets
// It is also meant to be used in situations where we do not need to build
// For example: Header only targets
class TargetInfo : public internal::SourceApi<TargetInfo>,
                   public internal::IncludeApi<TargetInfo>,
                   public internal::LibApi<TargetInfo>,
                   public internal::PchApi<TargetInfo>,
                   public internal::FlagApi<TargetInfo>,
                   public internal::DepsApi<TargetInfo>,
                   public internal::SyncApi<TargetInfo>,
                   public internal::TargetInfoGetter<TargetInfo> {
public:
  TargetInfo(const BaseToolchain &toolchain, const TargetEnv &env,
             const TargetConfig &config = TargetConfig())
      : toolchain_(toolchain), env_(env), config_(config) {
    Initialize();
  }

private:
  // Inputs
  friend class internal::SourceApi<TargetInfo>;
  friend class internal::IncludeApi<TargetInfo>;
  friend class internal::LibApi<TargetInfo>;
  friend class internal::PchApi<TargetInfo>;
  friend class internal::FlagApi<TargetInfo>;
  friend class internal::DepsApi<TargetInfo>;

  // Feature
  friend class internal::SyncApi<TargetInfo>;

  // Getters
  friend class internal::TargetInfoGetter<TargetInfo>;

protected:
  const BaseToolchain &toolchain_;
  TargetEnv env_;
  TargetConfig config_;

  //
  UserTargetSchema user_;
  FunctionLock lock_;

private:
  void Initialize();
};

typedef TargetInfo BaseTargetInfo;

} // namespace buildcc

#endif
