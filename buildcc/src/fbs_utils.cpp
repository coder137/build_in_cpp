#include "fbs_utils.h"

#include <filesystem>

#include "flatbuffers/util.h"

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
    // TODO, Improve this assert
    assert(false);
    break;
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

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<fbs::File>>>
fbs_utils_get_fbs_files(flatbuffers::FlatBufferBuilder &builder,
                        const std::unordered_set<std::string> &source_files) {
  std::vector<flatbuffers::Offset<fbs::File>> sources;
  for (const auto &source : source_files) {
    // uint64_t timestamp =
    // fs::last_write_time(source).time_since_epoch().count();
    auto current_file = buildcc::internal::File(source);
    auto fbs_file =
        fbs::CreateFileDirect(builder, current_file.GetFilename().c_str(),
                              current_file.GetLastWriteTimestamp());
    sources.push_back(fbs_file);
  }

  auto fbs_sources = builder.CreateVectorOfSortedTables(&sources);
  return fbs_sources;
}

} // namespace

namespace buildcc::internal {

// TODO, Improve with the direct API if possible
bool fbs_utils_save_fbs_target(const buildcc::Target &target) {
  flatbuffers::FlatBufferBuilder builder;
  auto fbs_name = builder.CreateString(target.GetName());
  auto fbs_relative_path =
      builder.CreateString(target.GetRelativePath().string());
  auto fbs_target_type = fbs_utils_get_fbs_target_type(target.GetTargetType());
  auto fbs_toolchain =
      fbs_utils_get_fbs_toolchain(builder, target.GetToolchain());

  auto fbs_source_files = fbs_utils_get_fbs_files(builder, target.GetSources());
  // TODO, Add more conversions here

  auto fbs_target =
      fbs::CreateTarget(builder, fbs_name, fbs_relative_path, fbs_target_type,
                        fbs_toolchain, fbs_source_files);
  fbs::FinishTargetBuffer(builder, fbs_target);

  auto file_path = target.GetRelativePath() / (target.GetName() + ".bin");
  return flatbuffers::SaveFile(file_path.string().c_str(),
                               (const char *)builder.GetBufferPointer(),
                               builder.GetSize(), true);
}

bool fbs_utils_fbs_load_target(const buildcc::Target &target,
                               fbs::TargetT *targetT) {
  return fbs_utils_fbs_load_target(target.GetName(), target.GetRelativePath(),
                                   targetT);
}

bool fbs_utils_fbs_load_target(const std::string &name,
                               const fs::path &relative_path,
                               fbs::TargetT *targetT) {
  auto file_path = relative_path / (name + ".bin");
  std::string buffer;
  bool is_loaded =
      flatbuffers::LoadFile(file_path.string().c_str(), true, &buffer);
  if (is_loaded) {
    auto target = fbs::GetTarget((const void *)buffer.c_str());
    target->UnPackTo(targetT);
  }

  return is_loaded;
}

bool fbs_utils_fbs_target_exists(const buildcc::Target &target) {
  return fbs_utils_fbs_target_exists(target.GetName(),
                                     target.GetRelativePath());
}

bool fbs_utils_fbs_target_exists(const std::string &name,
                                 const fs::path &relative_path) {
  auto file_path = relative_path / (name + ".bin");
  return fs::exists(file_path);
}

} // namespace buildcc::internal
