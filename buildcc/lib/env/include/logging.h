/*
 * Copyright 2021 Niket Naidu All rights reserved.
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

#ifndef ENV_INCLUDE_LOGGING_H_
#define ENV_INCLUDE_LOGGING_H_

#include <string>

namespace buildcc::env {

// NOTE, These numbers are mapped to spdlog::level::level_enum internally
enum class LogLevel {
  Trace = 0,
  Debug = 1,
  Info = 2,
  Warning = 3,
  Critical = 5,
  Error = Critical,
  Fatal = Critical,
};

void set_log_pattern(std::string_view pattern);
void set_log_level(LogLevel level);

// Logging functions
void log(LogLevel level, std::string_view tag, std::string_view message);
void log_trace(std::string_view tag, std::string_view message);
void log_debug(std::string_view tag, std::string_view message);
void log_info(std::string_view tag, std::string_view message);
void log_warning(std::string_view tag, std::string_view message);
void log_critical(std::string_view tag, std::string_view message);

} // namespace buildcc::env

#endif
