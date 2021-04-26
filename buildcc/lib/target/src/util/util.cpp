#include "internal/util.h"

#include "env.h"

namespace buildcc::internal {

// rechecks
bool is_previous_paths_different(const path_unordered_set &previous_paths,
                                 const path_unordered_set &current_paths) {
  for (const auto &file : previous_paths) {
    if (current_paths.find(file) == current_paths.end()) {
      return true;
    }
  }

  return false;
}

// Aggregates
std::string aggregate_include_dirs(
    const std::string &prefix,
    const buildcc::internal::path_unordered_set &include_dirs) {
  std::string idir{""};
  for (const auto &dirs : include_dirs) {
    idir += prefix + dirs.GetPathname().string() + " ";
  }
  return idir;
}

std::string aggregate(const std::vector<std::string> &list) {
  std::string agg = "";
  for (const auto &l : list) {
    agg += l + " ";
  }
  return agg;
}

std::string aggregate(const std::unordered_set<std::string> &list) {
  std::string agg = "";
  for (const auto &l : list) {
    agg += l + " ";
  }
  return agg;
}

std::string aggregate(const buildcc::internal::path_unordered_set &paths) {
  std::string agg = "";
  for (const auto &p : paths) {
    agg += p.GetPathname().string() + " ";
  }
  return agg;
}

} // namespace buildcc::internal
