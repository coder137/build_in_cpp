#include "base/target.h"

#include <filesystem>

#include "internal/assert_fatal.h"

#include "env.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

namespace fbs = schema::internal;

namespace {

fbs::TargetType get_fbs_target_type(buildcc::base::TargetType type) {
  fbs::TargetType target_type = fbs::TargetType_Executable;
  switch (type) {
  case buildcc::base::TargetType::Executable:
    break;
  case buildcc::base::TargetType::StaticLibrary:
    target_type = fbs::TargetType::TargetType_StaticLibrary;
    break;
  case buildcc::base::TargetType::DynamicLibrary:
    target_type = fbs::TargetType_DynamicLibrary;
    break;
  }
  return target_type;
}

// TODO, Complete this with additional flags
flatbuffers::Offset<fbs::Toolchain>
get_fbs_toolchain(flatbuffers::FlatBufferBuilder &builder,
                  const buildcc::Toolchain &toolchain) {
  // auto fbs_preprocessor_flags;
  // auto fbs_c_compile_flags;
  // auto fbs_cpp_compile_flags;
  // auto fbs_link_flags;
  return fbs::CreateToolchainDirect(builder, toolchain.GetName().c_str(),
                                    toolchain.GetCCompiler().c_str(),
                                    toolchain.GetCppCompiler().c_str());
}

std::vector<flatbuffers::Offset<fbs::Path>>
get_fbs_path(flatbuffers::FlatBufferBuilder &builder,
             const buildcc::internal::path_unordered_set &source_files) {
  std::vector<flatbuffers::Offset<fbs::Path>> sources;
  for (const auto &source : source_files) {
    auto fbs_file =
        fbs::CreatePathDirect(builder, source.GetPathname().string().c_str(),
                              source.GetLastWriteTimestamp());
    sources.push_back(fbs_file);
  }
  return sources;
}

} // namespace

namespace buildcc::base {

bool Target::Store() {
  env::log_trace(__FUNCTION__, name_);

  flatbuffers::FlatBufferBuilder builder;

  auto fbs_target_type = get_fbs_target_type(type_);
  auto fbs_toolchain = get_fbs_toolchain(builder, toolchain_);
  auto fbs_source_files = get_fbs_path(builder, current_source_files_);
  auto fbs_include_dirs = get_fbs_path(builder, current_include_dirs_);
  auto fbs_lib_deps = get_fbs_path(builder, current_lib_deps_);
  // lib_dirs

  auto fbs_target = fbs::CreateTargetDirect(
      builder, name_.c_str(), target_intermediate_dir_.string().c_str(),
      fbs_target_type, fbs_toolchain, &fbs_source_files, &fbs_include_dirs,
      &fbs_lib_deps);
  fbs::FinishTargetBuffer(builder, fbs_target);

  auto file_path = GetBinaryPath();
  return flatbuffers::SaveFile(file_path.string().c_str(),
                               (const char *)builder.GetBufferPointer(),
                               builder.GetSize(), true);
}

} // namespace buildcc::base
