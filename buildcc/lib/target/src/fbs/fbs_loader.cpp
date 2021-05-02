#include "internal/fbs_loader.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

#include "logging.h"

namespace fbs = schema::internal;

namespace {
void Extract(
    const flatbuffers::Vector<flatbuffers::Offset<schema::internal::Path>>
        *fbs_paths,
    buildcc::internal::path_unordered_set &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(buildcc::internal::Path::CreateNewPath(
        iter->pathname()->c_str(), iter->last_write_timestamp()));
  }
}

void Extract(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>
                 *fbs_paths,
             std::unordered_set<std::string> &out) {
  if (fbs_paths == nullptr) {
    return;
  }

  for (auto iter = fbs_paths->begin(); iter != fbs_paths->end(); iter++) {
    out.insert(iter->str());
  }
}

// TODO, ExtractLibs

} // namespace

namespace buildcc::internal {

// Public functions
bool FbsLoader::Load() {
  env::log_trace(name_, __FUNCTION__);

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
  Extract(target->source_files(), loaded_sources_);
  Extract(target->header_files(), loaded_headers_);
  Extract(target->lib_deps(), loaded_lib_deps_);

  Extract(target->external_lib_deps(), loaded_external_lib_dirs_);

  Extract(target->include_dirs(), loaded_include_dirs_);
  Extract(target->lib_dirs(), loaded_lib_dirs_);

  Extract(target->preprocessor_flags(), loaded_preprocessor_flags_);
  Extract(target->c_compile_flags(), loaded_c_compile_flags_);
  Extract(target->cpp_compile_flags(), loaded_cpp_compile_flags_);
  Extract(target->link_flags(), loaded_link_flags_);

  loaded_ = true;
  return true;
}

// Private functions
void FbsLoader::Initialize() {}

} // namespace buildcc::internal
