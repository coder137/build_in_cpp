#include "internal/schema_loader.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

#include "spdlog/spdlog.h"

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
bool SchemaLoader::Load() {
  spdlog::trace(__FUNCTION__);

  auto file_path = relative_path_ / (name_ + ".bin");
  std::string buffer;
  bool is_loaded =
      flatbuffers::LoadFile(file_path.string().c_str(), true, &buffer);
  if (!is_loaded) {
    return false;
  }
  auto target = fbs::GetTarget((const void *)buffer.c_str());
  ExtractPaths(target->source_files(), &loaded_sources_);
  ExtractPaths(target->include_dirs(), &loaded_include_dirs_);
  // TODO, ExtractLibDirs
  return true;
}

// Private functions
void SchemaLoader::Initialize() {
  spdlog::set_level(spdlog::level::trace);
  spdlog::trace(__FUNCTION__);
}

} // namespace buildcc::internal
