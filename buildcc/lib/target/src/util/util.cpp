#include "internal/util.h"

#include "env.h"

namespace {

std::string aggregate_string_vector(const std::vector<std::string> &list) {
  std::string files = "";
  for (const auto &l : list) {
    files += l + " ";
  }
  return files;
}

std::string aggregate_path_unordered_set(
    const buildcc::internal::path_unordered_set &paths) {
  std::string aggregated_path = "";
  for (const auto &p : paths) {
    aggregated_path += p.GetPathname().string() + " ";
  }
  return aggregated_path;
}

} // namespace

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
std::string
aggregate_compiled_sources(const std::vector<std::string> &compiled_sources) {
  return aggregate_string_vector(compiled_sources);
}

std::string aggregate_preprocessor_flags(
    const std::vector<std::string> &preprocessor_flags) {
  return aggregate_string_vector(preprocessor_flags);
}

std::string
aggregate_compile_flags(const std::vector<std::string> &compile_flags) {
  return aggregate_string_vector(compile_flags);
}

std::string
aggregate_link_flags(const std::vector<std::string> &compiled_sources) {
  return aggregate_string_vector(compiled_sources);
}

std::string aggregate_lib_deps(const path_unordered_set &lib_deps) {
  return aggregate_path_unordered_set(lib_deps);
}

} // namespace buildcc::internal
