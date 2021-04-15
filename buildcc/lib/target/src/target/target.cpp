#include "target.h"

// Internal
#include "internal/util.h"

// Env
#include "assert_fatal.h"

namespace fs = std::filesystem;

namespace {

bool IsValidTargetType(buildcc::base::TargetType type) {
  switch (type) {
  case buildcc::base::TargetType::Executable:
  case buildcc::base::TargetType::StaticLibrary:
  case buildcc::base::TargetType::DynamicLibrary:
    return true;
    break;
  default:
    return false;
    break;
  }
}

} // namespace

namespace buildcc::base {

// * Load
// TODO, Verify things that cannot be changed
// * Compile
// Include directories dependencies
// * Link
// Library dependencies
void Target::Build() {
  env::log_trace(__FUNCTION__, name_);

  const bool is_loaded = loader_.Load();
  if (!is_loaded) {
    BuildCompile();
  } else {
    BuildRecompile();
  }

  dirty_ = false;
}

void Target::BuildCompile() {
  const std::vector<std::string> compiled_sources = CompileSources();
  BuildTarget(compiled_sources);
  Store();
}

// * Target rebuild depends on
// TODO, Toolchain name
// TODO, Toolchain preprocessor flags
// TODO, Toolchain compile flags
// TODO, Toolchain link flags
// TODO, Target preprocessor flags
// TODO, Target compile flags
// TODO, Target link flags
// Target source files
// Target include dirs
// Target library dependencies
// TODO, Target library directories
void Target::BuildRecompile() {

  // * Completely compile sources if any of the following change
  // TODO, Toolchain, ASM, C, C++ compiler
  // TODO, Toolchain preprocessor flags
  // TODO, Toolchain compile flags
  // TODO, Target preprocessor flags
  // TODO, Target compile flags
  // Target include dirs
  RecheckIncludeDirs();

  // * Compile sources
  std::vector<std::string> compiled_sources;
  if (dirty_) {
    compiled_sources = CompileSources();
  } else {
    compiled_sources = RecompileSources();
  }

  // * Completely rebuild target / link if any of the following change
  // TODO, Toolchain link flags
  // TODO, Target link flags
  // Target compiled source files either during Compile / Recompile
  // Target library dependencies
  // TODO, Target library directories
  RecheckLibDeps();
  if (dirty_) {
    BuildTarget(compiled_sources);
    Store();
  }
}

// PROTECTED

// Getters
std::string Target::GetCompiledSourceName(const fs::path &source) const {
  const auto output_filename =
      GetTargetIntermediateDir() / (source.filename().string() + ".o");
  return output_filename.string();
}

std::string Target::GetCompiler(const fs::path &source) const {
  // .cpp -> GetCppCompiler
  // .c / .asm -> GetCCompiler
  std::string compiler = source.extension() == ".cpp"
                             ? toolchain_.GetCppCompiler()
                             : toolchain_.GetCCompiler();
  return compiler;
}

// PRIVATE

void Target::Initialize() {
  env::assert_fatal(
      env::is_init(),
      "Environment is not initialized. Use the buildcc::env::init API");
  env::assert_fatal(IsValidTargetType(type_), "Invalid Target Type");
  fs::create_directories(target_intermediate_dir_);
}

// Linking
std::vector<std::string>
Target::Link(const std::string &output_target,
             const std::string &aggregated_link_flags,
             const std::string &aggregated_compiled_sources,
             const std::string &aggregated_lib_deps) {
  return {
      toolchain_.GetCppCompiler(),
      aggregated_link_flags,
      aggregated_compiled_sources,
      "-o",
      output_target,
      aggregated_lib_deps,
  };
}

void Target::BuildTarget(const std::vector<std::string> &compiled_sources) {
  env::log_trace(__FUNCTION__, name_);

  // Add compiled sources
  const std::string aggregated_link_flags =
      internal::aggregate_link_flags(toolchain_.GetLinkFlags());
  const std::string aggregated_compiled_sources =
      internal::aggregate_compiled_sources(compiled_sources);

  const std::string aggregated_lib_deps =
      internal::aggregate_lib_deps(current_lib_deps_);

  // Final Target
  // TODO, Improve this logic
  // Select cpp compiler for building target only if there is .cpp file
  // added
  // Else use c compiler
  const fs::path target = GetTargetPath();

  bool success =
      internal::command(Link(target.string(), aggregated_link_flags,
                             aggregated_compiled_sources, aggregated_lib_deps));

  env::assert_fatal(success, "Compilation failed for: " + GetName());
}

// Rechecks
void Target::Recheck(const internal::path_unordered_set &previous_path,
                     const internal::path_unordered_set &current_path) {
  // * Compile sources / Target already requires rebuild
  if (dirty_) {
    return;
  }

  // * Old path is removed
  const bool removed =
      internal::is_previous_paths_different(previous_path, current_path);
  if (removed) {
    PathRemoved();
    dirty_ = true;
    return;
  }

  for (auto &path : current_path) {
    auto iter = previous_path.find(path);

    if (iter == previous_path.end()) {
      // * New path added
      dirty_ = true;
      PathAdded();
      break;
    } else {
      // * Path is updated
      if (path.GetLastWriteTimestamp() > iter->GetLastWriteTimestamp()) {
        dirty_ = true;
        PathUpdated();
        break;
      } else {
        // * Do nothing
      }
    }
  }
}

} // namespace buildcc::base
