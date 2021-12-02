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

#ifndef ARGS_PERSISTENT_STORAGE_H_
#define ARGS_PERSISTENT_STORAGE_H_

#include <functional>
#include <string>
#include <typeinfo>
#include <unordered_map>

#include "env/assert_fatal.h"

namespace buildcc {

class PersistentStorage {
public:
  PersistentStorage() {}
  ~PersistentStorage() {
    for (const auto &ptr_iter : ptrs_) {
      ptr_iter.second.destructor();
    }
    ptrs_.clear();
    env::assert_fatal(ptrs_.empty(), "Memory not deallocated");
  }

  template <typename T, typename... Params>
  void Add(const std::string &identifier, Params &&...params) {
    T *ptr = new T(std::forward<Params &&>(params)...);
    PtrMetadata metadata;
    metadata.ptr = (void *)ptr;
    metadata.typeid_name = typeid(T).name();
    metadata.destructor = [&, identifier]() {
      env::log_info("Removing", identifier);
      Remove<T>(identifier);
    };
    ptrs_.emplace(identifier, metadata);
  }

  template <typename T> T &Ref(const std::string &identifier) {
    PtrMetadata &metadata = ptrs_.at(identifier);
    env::assert_fatal(
        typeid(T).name() == metadata.typeid_name,
        fmt::format("Wrong type, expects: {}", metadata.typeid_name));
    T *p = (T *)metadata.ptr;
    return *p;
  }

  template <typename T> const T &ConstRef(const std::string &identifier) const {
    return Get<T>(identifier);
  }

private:
  /**
   * @brief
   * @param ptr Can hold data of any type
   * @param typeid_name We cannot store a template type so this is the next best
   * thing
   * @param desstructor Destructor callback to delete ptr
   */
  struct PtrMetadata {
    void *ptr;
    std::string typeid_name;
    std::function<void(void)> destructor;
  };

private:
  template <typename T> void Remove(const std::string &identifier) {
    T *ptr = (T *)(ptrs_.at(identifier).ptr);
    delete ptr;
  }

  std::unordered_map<std::string, PtrMetadata> ptrs_;
};

} // namespace buildcc

#endif
