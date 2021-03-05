#include "schema_loader.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace fbs = schema::internal;

namespace {
void ExtractSources(
    const flatbuffers::Vector<flatbuffers::Offset<schema::internal::Path>>
        *fbs_sources,
    std::unordered_set<buildcc::internal::Path, buildcc::internal::PathHash>
        *loaded_sources) {
  for (auto iter = fbs_sources->begin(); iter != fbs_sources->end(); iter++) {
    loaded_sources->insert(buildcc::internal::Path::CreateNewPath(
        iter->pathname()->c_str(), iter->last_write_timestamp()));
  }
}

// TODO, ExtractIncludeDirs
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
  ExtractSources(target->source_files(), &loaded_sources_);
  // TODO, ExtractDirs
  // TODO, ExtractLibDirs

  return true;
}

// Private functions
void SchemaLoader::Initialize() {
  spdlog::set_level(spdlog::level::trace);
  spdlog::trace(__FUNCTION__);
}

} // namespace buildcc::internal
