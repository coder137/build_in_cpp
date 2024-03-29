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

#include "target/api/target_getter.h"

#include "target/target.h"

namespace buildcc::internal {

// Target State
template <typename T> const TargetState &TargetGetter<T>::GetState() const {
  const auto &t = static_cast<const T &>(*this);
  return t.state_;
}

template <typename T> bool TargetGetter<T>::IsBuilt() const {
  const auto &t = static_cast<const T &>(*this);
  return t.state_.IsBuilt();
}

// Target Config
template <typename T> const TargetConfig &TargetGetter<T>::GetConfig() const {
  const auto &t = static_cast<const T &>(*this);
  return t.config_;
}

template <typename T> const fs::path &TargetGetter<T>::GetBinaryPath() const {
  const auto &t = static_cast<const T &>(*this);
  return t.serialization_.GetSerializedFile();
}

template <typename T> const fs::path &TargetGetter<T>::GetTargetPath() const {
  const auto &t = static_cast<const T &>(*this);
  return t.link_target_.GetOutput();
}

template <typename T>
const fs::path &TargetGetter<T>::GetPchHeaderPath() const {
  const auto &t = static_cast<const T &>(*this);
  return t.compile_pch_.GetHeaderPath();
}

template <typename T>
const fs::path &TargetGetter<T>::GetPchCompilePath() const {
  const auto &t = static_cast<const T &>(*this);
  return t.compile_pch_.GetCompilePath();
}

template <typename T> const std::string &TargetGetter<T>::GetName() const {
  const auto &t = static_cast<const T &>(*this);
  return t.name_;
}

template <typename T> const Toolchain &TargetGetter<T>::GetToolchain() const {
  const auto &t = static_cast<const T &>(*this);
  return t.toolchain_;
}

template <typename T> TargetType TargetGetter<T>::GetType() const {
  const auto &t = static_cast<const T &>(*this);
  return t.type_;
}

template <typename T>
const std::string &
TargetGetter<T>::GetCompileCommand(const fs::path &source) const {
  const auto &t = static_cast<const T &>(*this);
  return t.compile_object_.GetObjectData(source).command;
}

template <typename T>
const std::string &TargetGetter<T>::GetLinkCommand() const {
  const auto &t = static_cast<const T &>(*this);
  return t.link_target_.GetCommand();
}

template class TargetGetter<BaseTarget>;

} // namespace buildcc::internal
