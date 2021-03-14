#include "internal/fbs_utils.h"

#include <filesystem>

#include "internal/fatal_assert.h"

#include "flatbuffers/util.h"
#include "target_generated.h"

namespace fbs = schema::internal;

namespace {

fbs::TargetType fbs_utils_get_fbs_target_type(buildcc::TargetType type) {
  fbs::TargetType target_type = fbs::TargetType_Executable;
  switch (type) {
  case buildcc::TargetType::Executable:
    break;
  case buildcc::TargetType::StaticLibrary:
    target_type = fbs::TargetType::TargetType_StaticLibrary;
    break;
  case buildcc::TargetType::DynamicLibrary:
    target_type = fbs::TargetType_DynamicLibrary;
    break;
  default:
    buildcc::assert_fatal_true(false, "Not a valid TargetType");
  }
  return target_type;
}

flatbuffers::Offset<fbs::Toolchain>
fbs_utils_get_fbs_toolchain(flatbuffers::FlatBufferBuilder &builder,
                            const buildcc::Toolchain &toolchain) {
  auto fbs_toolchain_name = builder.CreateString(toolchain.GetName());
  auto fbs_c_compiler = builder.CreateString(toolchain.GetCCompiler());
  auto fbs_cpp_compiler = builder.CreateString(toolchain.GetCppCompiler());
  // auto fbs_preprocessor_flags;
  // auto fbs_c_compile_flags;
  // auto fbs_cpp_compile_flags;
  // auto fbs_link_flags;
  return fbs::CreateToolchain(builder, fbs_toolchain_name, fbs_c_compiler,
                              fbs_cpp_compiler);
}

std::vector<flatbuffers::Offset<fbs::Path>> fbs_utils_get_fbs_path(
    flatbuffers::FlatBufferBuilder &builder,
    const buildcc::internal::path_unordered_set &source_files) {
  std::vector<flatbuffers::Offset<fbs::Path>> sources;
  for (const auto &source : source_files) {
    auto fbs_file = fbs::CreatePathDirect(builder, source.GetPathname().c_str(),
                                          source.GetLastWriteTimestamp());
    sources.push_back(fbs_file);
  }
  return sources;
}

} // namespace

namespace buildcc::internal {

bool fbs_utils_store_target(const std::string &name,
                            const fs::path &relative_path,
                            TargetType target_type, const Toolchain &toolchain,
                            const path_unordered_set &source_files,
                            const path_unordered_set &include_dirs) {
  flatbuffers::FlatBufferBuilder builder;

  auto fbs_target_type = fbs_utils_get_fbs_target_type(target_type);
  auto fbs_toolchain = fbs_utils_get_fbs_toolchain(builder, toolchain);
  auto fbs_source_files = fbs_utils_get_fbs_path(builder, source_files);
  auto fbs_include_dirs = fbs_utils_get_fbs_path(builder, include_dirs);

  auto fbs_target = fbs::CreateTargetDirect(
      builder, name.c_str(), relative_path.string().c_str(), fbs_target_type,
      fbs_toolchain, &fbs_source_files, &fbs_include_dirs);
  fbs::FinishTargetBuffer(builder, fbs_target);

  auto file_path = relative_path / (name + ".bin");
  return flatbuffers::SaveFile(file_path.string().c_str(),
                               (const char *)builder.GetBufferPointer(),
                               builder.GetSize(), true);
}

} // namespace buildcc::internal
