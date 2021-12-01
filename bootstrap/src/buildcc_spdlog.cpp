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

#include "bootstrap/buildcc_spdlog.h"

namespace buildcc {

void spdlog_ho_cb(TargetInfo &info) {
  info.AddIncludeDir("include");
  info.GlobHeaders("include/spdlog");
  info.GlobHeaders("include/spdlog/cfg");
  info.GlobHeaders("include/spdlog/details");
  info.GlobHeaders("include/spdlog/fmt");
  info.GlobHeaders("include/spdlog/sinks");
}

} // namespace buildcc
