#include "internal/fbs_loader.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

#include "env.h"

namespace fbs = schema::internal;

namespace {
void ExtractPaths(
    const flatbuffers::Vector<flatbuffers::Offset<schema::internal::Path>>
        *fbs_paths,
    buildcc::internal::path_unordered_set *loaded_sources) {
  if (fbs_paths == nullptr || loaded_sources == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    loaded_sources->insert(buildcc::internal::Path::CreateNewPath(
        iter->pathname()->c_str(), iter->last_write_timestamp()));
  }
}

// TODO, ExtractLibs

} // namespace

namespace buildcc::internal {

// Public functions
bool FbsLoader::Load() {
  env::log_trace(__FUNCTION__, name_);

  auto file_path = GetBinaryPath();
  std::string buffer;
  bool is_loaded =
      flatbuffers::LoadFile(file_path.string().c_str(), true, &buffer);
  if (!is_loaded) {
    return false;
  }
  auto target = fbs::GetTarget((const void *)buffer.c_str());
  // target->name()->c_str();
  // target->relative_path()->c_str();
  // target->type();
  // target->toolchain();
  ExtractPaths(target->source_files(), &loaded_sources_);
  ExtractPaths(target->include_dirs(), &loaded_include_dirs_);
  ExtractPaths(target->lib_deps(), &loaded_lib_deps_);
  // target->lib_dirs();

  loaded_ = true;
  return true;
}

// Private functions
void FbsLoader::Initialize() {}

} // namespace buildcc::internal
