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

#ifndef BOOTSTRAP_BUILD_BUILDCC_H_
#define BOOTSTRAP_BUILD_BUILDCC_H_

#include "buildcc.h"

namespace buildcc {

void schema_gen_cb(BaseGenerator &generator, const BaseTarget &flatc_exe);

void buildcc_use_existing_targets_cb(
    BaseTarget &target, const BaseGenerator &schema_gen,
    const TargetInfo &flatbuffers_ho, const TargetInfo &fmt_ho,
    const TargetInfo &spdlog_ho, const TargetInfo &cli11_ho,
    const TargetInfo &taskflow_ho, const BaseTarget &tpl);

} // namespace buildcc

#endif
