#ifndef BUILDCC_FBS_FBS_UTILS_H_
#define BUILDCC_FBS_FBS_UTILS_H_

#include <filesystem>
#include <string>

#include "buildcc.h"
#include "toolchain.h"

// Third party
#include "target_generated.h"

namespace fs = std::filesystem;
namespace fbs = schema::internal;

namespace buildcc::internal {

/**
 * @brief Creates a new FBS Target
 * Writes to a <name>.bin binary file
 *
 * @param target
 */
bool fbs_utils_save_fbs_target(const buildcc::Target &target);

/**
 * @brief
 *
 * @param target
 * @return true
 * @return false
 */
bool fbs_utils_fbs_load_target(const buildcc::Target &target,
                               fbs::TargetT *targetT);
bool fbs_utils_fbs_load_target(const std::string &name,
                               const fs::path &relative_path,
                               fbs::TargetT *targetT);

/**
 * @brief Checks if an FBS Target exists
 * Checks against <name>.bin binary file
 *
 * @param target
 * @return true If file exists
 * @return false If file does not exist
 */
bool fbs_utils_fbs_target_exists(const buildcc::Target &target);
bool fbs_utils_fbs_target_exists(const std::string &name,
                                 const fs::path &relative_path);

} // namespace buildcc::internal

#endif
