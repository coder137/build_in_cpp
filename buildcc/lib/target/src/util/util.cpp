#include "internal/util.h"

#include "assert_fatal.h"
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

// Additions
bool add_path(const fs::path &path, path_unordered_set &stored_paths) {
  env::assert_fatal(fs::exists(path), path.string() + " not found");
  auto current_file = buildcc::internal::Path::CreateExistingPath(path);

  // TODO, Note, we might not require this check
  env::assert_fatal(stored_paths.find(current_file) == stored_paths.end(),
                    path.string() + " duplicate found");

  auto [_, added] = stored_paths.insert(current_file);
  return added;
}

std::string quote(const std::string &str) {
  if (str.find(" ") == std::string::npos) {
    return str;
  }

  return "\"" + str + "\"";
}

// Aggregates

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
