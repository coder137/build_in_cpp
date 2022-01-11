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

#include "env/logging.h"

#include "spdlog/spdlog.h"

namespace buildcc::env {

void set_log_pattern(std::string_view pattern) {
  spdlog::set_pattern(pattern.data());
}

void set_log_level(LogLevel level) {
  spdlog::set_level((spdlog::level::level_enum)level);
}

void log(LogLevel level, std::string_view tag, std::string_view message) {
  spdlog::log((spdlog::level::level_enum)level, "[{}]: {}", tag, message);
}
void log_trace(std::string_view tag, std::string_view message) {
  log(LogLevel::Trace, tag, message);
}
void log_debug(std::string_view tag, std::string_view message) {
  log(LogLevel::Debug, tag, message);
}
void log_info(std::string_view tag, std::string_view message) {
  log(LogLevel::Info, tag, message);
}
void log_warning(std::string_view tag, std::string_view message) {
  log(LogLevel::Warning, tag, message);
}
void log_critical(std::string_view tag, std::string_view message) {
  log(LogLevel::Critical, tag, message);
}

} // namespace buildcc::env
