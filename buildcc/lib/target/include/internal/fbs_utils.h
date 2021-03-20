#ifndef TARGET_INCLUDE_INTERNAL_FBS_UTILS_H_
#define TARGET_INCLUDE_INTERNAL_FBS_UTILS_H_

#include <filesystem>
#include <string>

#include "path.h"
#include "target_type.h"
#include "toolchain.h"

namespace fs = std::filesystem;

namespace buildcc::internal {

/**
 * @brief
 * TODO, Add library options
 *
 * @param name
 * @param relative_path
 * @param target_type
 * @param toolchain
 * @param source_files
 * @param include_dirs
 * @return true
 * @return false
 */
bool fbs_utils_store_target(const std::string &name,
                            const fs::path &relative_path,
                            TargetType target_type, const Toolchain &toolchain,
                            const path_unordered_set &source_files,
                            const path_unordered_set &include_dirs);

} // namespace buildcc::internal

#endif
