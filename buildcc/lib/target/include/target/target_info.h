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

#ifndef TARGET_TARGET_INFO_H_
#define TARGET_TARGET_INFO_H_

#include <string>

#include "target/base/target_storer.h"
#include "target/common/target_config.h"
#include "target/common/target_env.h"
#include "target/common/target_state.h"

#include "target/api/copy_api.h"
#include "target/api/deps_api.h"
#include "target/api/flag_api.h"
#include "target/api/include_api.h"
#include "target/api/lib_api.h"
#include "target/api/pch_api.h"

#include "target/api/source_api.h"

#include "target/api/target_info_getter.h"

namespace buildcc::base {

//
class TargetInfo : public SourceApi<TargetInfo>,
                   public IncludeApi<TargetInfo>,
                   public LibApi<TargetInfo>,
                   public PchApi<TargetInfo>,
                   public FlagApi<TargetInfo>,
                   public DepsApi<TargetInfo>,
                   public CopyApi<TargetInfo>,
                   public TargetInfoGetter<TargetInfo> {
public:
  TargetInfo(const TargetEnv &env, const TargetConfig &config = TargetConfig())
      : env_(env), config_(config) {}

private:
  // Inputs
  friend class SourceApi<TargetInfo>;
  friend class IncludeApi<TargetInfo>;
  friend class LibApi<TargetInfo>;
  friend class PchApi<TargetInfo>;
  friend class FlagApi<TargetInfo>;
  friend class DepsApi<TargetInfo>;

  // Feature
  friend class CopyApi<TargetInfo>;

  // Getters
  friend class TargetInfoGetter<TargetInfo>;

protected:
  TargetEnv env_;
  TargetConfig config_;

  internal::TargetStorer storer_;
  TargetState state_;
};

} // namespace buildcc::base

namespace buildcc {

typedef base::TargetInfo TargetInfo;

}

#endif
