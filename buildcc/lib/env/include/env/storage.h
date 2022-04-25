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

#ifndef ENV_STORAGE_H_
#define ENV_STORAGE_H_

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

#include "env/assert_fatal.h"

#include "fmt/format.h"

namespace buildcc {

class ScopedStorage {
public:
  ScopedStorage() {}
  ~ScopedStorage() {
    for (const auto &ptr_iter : ptrs_) {
      ptr_iter.second.destructor();
    }
    ptrs_.clear();
    env::assert_fatal(ptrs_.empty(), "Memory not deallocated");
  }

  ScopedStorage(const ScopedStorage &) = delete;

  template <typename T, typename... Params>
  T &Add(const std::string &identifier, Params &&...params) {
    T *ptr = new T(std::forward<Params>(params)...);
    env::assert_fatal(ptr != nullptr, "System out of memory");

    PtrMetadata metadata;
    metadata.ptr = (void *)ptr;
    metadata.typeid_name = typeid(T).name();
    metadata.destructor = [this, identifier, ptr]() {
      env::log_trace("Cleaning", identifier);
      Remove<T>(ptr);
    };
    ptrs_.emplace(identifier, metadata);
    return *ptr;
  }

  template <typename T> const T &ConstRef(const std::string &identifier) const {
    env::assert_fatal(Contains(identifier),
                      fmt::format("Could not find '{}'", identifier));
    const PtrMetadata &metadata = ptrs_.at(identifier);
    env::assert_fatal(
        typeid(T).name() == metadata.typeid_name,
        fmt::format("Wrong type, expects: {}", metadata.typeid_name));
    const T *p = (const T *)metadata.ptr;
    return *p;
  }

  // https://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func/123995
  template <typename T> T &Ref(const std::string &identifier) {
    return const_cast<T &>(
        static_cast<const ScopedStorage &>(*this).ConstRef<T>(identifier));
  }

  bool Contains(const std::string &identifier) const {
    return (ptrs_.find(identifier) != ptrs_.end());
  }

  template <typename T> bool Valid(const std::string &identifier) const {
    if (!Contains(identifier)) {
      return false;
    }
    const PtrMetadata &metadata = ptrs_.at(identifier);
    if (typeid(T).name() != metadata.typeid_name) {
      return false;
    }
    return true;
  }

protected:
  template <typename T> void Remove(T *ptr) { delete ptr; }

private:
  /**
   * @brief
   * @param ptr Can hold data of any type
   * @param typeid_name We cannot store a template type so this is the next
   * best thing
   * @param desstructor Destructor callback to delete ptr
   */
  struct PtrMetadata {
    void *ptr;
    std::string typeid_name;
    std::function<void(void)> destructor;
  };

private:
  std::unordered_map<std::string, PtrMetadata> ptrs_;
};

class Storage {
public:
  Storage() = delete;
  Storage(const Storage &) = delete;
  Storage(Storage &&) = delete;

  static void Init() { internal_ = std::make_unique<ScopedStorage>(); }
  static void Deinit() { internal_.reset(nullptr); }

  template <typename T, typename... Params>
  static T &Add(const std::string &identifier, Params &&...params) {
    return Ref().Add<T, Params...>(identifier, std::forward<Params>(params)...);
  }

  template <typename T>
  static const T &ConstRef(const std::string &identifier) {
    return Ref().ConstRef<T>(identifier);
  }

  template <typename T> static T &Ref(const std::string &identifier) {
    return Ref().Ref<T>(identifier);
  }

  static bool Contains(const std::string &identifier) {
    return Ref().Contains(identifier);
  }

  template <typename T> static bool Valid(const std::string &identifier) {
    return Ref().Valid<T>(identifier);
  }

private:
  static ScopedStorage &Ref();

private:
  static std::unique_ptr<ScopedStorage> internal_;
};

} // namespace buildcc

#endif
