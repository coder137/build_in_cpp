#include "env/host_os_util.h"
#include "env/util.h"

#include "expect_command.h"
#include "mock_command_copier.h"

#include "toolchains/toolchain_specialized.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(ToolchainSpecializedTestGroup)
{
  void setup() {
  }
  void teardown() {
    mock().clear();
  }
};
// clang-format on

TEST(ToolchainSpecializedTestGroup, GCC) {
  buildcc::Toolchain_gcc gcc;
  STRCMP_EQUAL(gcc.GetName().c_str(), "gcc");
  STRCMP_EQUAL(gcc.GetAssembler().c_str(), "as");
  STRCMP_EQUAL(gcc.GetCCompiler().c_str(), "gcc");
  STRCMP_EQUAL(gcc.GetCppCompiler().c_str(), "g++");
  STRCMP_EQUAL(gcc.GetArchiver().c_str(), "ar");
  STRCMP_EQUAL(gcc.GetLinker().c_str(), "ld");

  fs::path current_directory = fs::current_path();
  buildcc::ToolchainFindConfig find_config;
  find_config.env_vars.clear();
  find_config.absolute_search_paths.insert(current_directory);

  std::vector<std::string> version_stdout{"version"};
  std::vector<std::string> arch_stdout{"arch"};
  buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout);
  auto info = gcc.Verify(find_config);
  STRCMP_EQUAL(info.compiler_version.c_str(), "version");
  STRCMP_EQUAL(info.target_arch.c_str(), "arch");
}

TEST(ToolchainSpecializedTestGroup, GCC_Fail) {
  buildcc::Toolchain_gcc gcc;
  STRCMP_EQUAL(gcc.GetName().c_str(), "gcc");
  STRCMP_EQUAL(gcc.GetAssembler().c_str(), "as");
  STRCMP_EQUAL(gcc.GetCCompiler().c_str(), "gcc");
  STRCMP_EQUAL(gcc.GetCppCompiler().c_str(), "g++");
  STRCMP_EQUAL(gcc.GetArchiver().c_str(), "ar");
  STRCMP_EQUAL(gcc.GetLinker().c_str(), "ld");

  fs::path current_directory = fs::current_path();
  buildcc::ToolchainFindConfig find_config;
  find_config.env_vars.clear();
  find_config.absolute_search_paths.insert(current_directory);

  {
    std::vector<std::string> version_stdout{"version"};
    std::vector<std::string> arch_stdout{"arch"};
    buildcc::env::m::CommandExpect_Execute(1, false, &version_stdout);
    buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout);
    CHECK_THROWS(std::exception, gcc.Verify(find_config));
  }

  {
    std::vector<std::string> version_stdout;
    std::vector<std::string> arch_stdout{"arch"};
    buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout);
    buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout);
    CHECK_THROWS(std::exception, gcc.Verify(find_config));
  }

  {
    std::vector<std::string> version_stdout{"version"};
    std::vector<std::string> arch_stdout{"arch"};
    buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout);
    buildcc::env::m::CommandExpect_Execute(1, false, &arch_stdout);
    CHECK_THROWS(std::exception, gcc.Verify(find_config));
  }

  {
    std::vector<std::string> version_stdout{"version"};
    std::vector<std::string> arch_stdout{};
    buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout);
    buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout);
    CHECK_THROWS(std::exception, gcc.Verify(find_config));
  }
}

TEST(ToolchainSpecializedTestGroup, MINGW) {
  buildcc::Toolchain_mingw gcc;
  STRCMP_EQUAL(gcc.GetName().c_str(), "gcc");
  STRCMP_EQUAL(gcc.GetAssembler().c_str(), "as");
  STRCMP_EQUAL(gcc.GetCCompiler().c_str(), "gcc");
  STRCMP_EQUAL(gcc.GetCppCompiler().c_str(), "g++");
  STRCMP_EQUAL(gcc.GetArchiver().c_str(), "ar");
  STRCMP_EQUAL(gcc.GetLinker().c_str(), "ld");

  fs::path current_directory = fs::current_path();
  buildcc::ToolchainFindConfig find_config;
  find_config.env_vars.clear();
  find_config.absolute_search_paths.insert(current_directory);

  std::vector<std::string> version_stdout{"version"};
  std::vector<std::string> arch_stdout{"arch"};
  buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout);
  auto info = gcc.Verify(find_config);
  STRCMP_EQUAL(info.compiler_version.c_str(), "version");
  STRCMP_EQUAL(info.target_arch.c_str(), "arch");
}

TEST(ToolchainSpecializedTestGroup, MSVC) {
  buildcc::Toolchain_msvc msvc;
  STRCMP_EQUAL(msvc.GetName().c_str(), "msvc");
  STRCMP_EQUAL(msvc.GetAssembler().c_str(), "cl");
  STRCMP_EQUAL(msvc.GetCCompiler().c_str(), "cl");
  STRCMP_EQUAL(msvc.GetCppCompiler().c_str(), "cl");
  STRCMP_EQUAL(msvc.GetArchiver().c_str(), "lib");
  STRCMP_EQUAL(msvc.GetLinker().c_str(), "link");

  const auto &toolchain_config = msvc.GetConfig();
  STRCMP_EQUAL(toolchain_config.obj_ext.c_str(), ".obj");
  STRCMP_EQUAL(toolchain_config.pch_header_ext.c_str(), ".h");
  STRCMP_EQUAL(toolchain_config.pch_compile_ext.c_str(), ".pch");
  STRCMP_EQUAL(toolchain_config.prefix_include_dir.c_str(), "/I");
  STRCMP_EQUAL(toolchain_config.prefix_lib_dir.c_str(), "/LIBPATH:");

  fs::path current_directory = fs::current_path();
  buildcc::ToolchainFindConfig find_config;
  find_config.env_vars.clear();
  find_config.absolute_search_paths.insert(current_directory);

  CHECK_EQUAL(putenv("VSCMD_VER=version"), 0);
  CHECK_EQUAL(putenv("VSCMD_ARG_HOST_ARCH=host"), 0);
  CHECK_EQUAL(putenv("VSCMD_ARG_TGT_ARCH=target"), 0);
  auto info = msvc.Verify(find_config);
  STRCMP_EQUAL(info.compiler_version.c_str(), "version");
  STRCMP_EQUAL(info.target_arch.c_str(), "host_target");
}

TEST(ToolchainSpecializedTestGroup, MSVC_Fail) {
  buildcc::Toolchain_msvc msvc;
  STRCMP_EQUAL(msvc.GetName().c_str(), "msvc");
  STRCMP_EQUAL(msvc.GetAssembler().c_str(), "cl");
  STRCMP_EQUAL(msvc.GetCCompiler().c_str(), "cl");
  STRCMP_EQUAL(msvc.GetCppCompiler().c_str(), "cl");
  STRCMP_EQUAL(msvc.GetArchiver().c_str(), "lib");
  STRCMP_EQUAL(msvc.GetLinker().c_str(), "link");

  const auto &toolchain_config = msvc.GetConfig();
  STRCMP_EQUAL(toolchain_config.obj_ext.c_str(), ".obj");
  STRCMP_EQUAL(toolchain_config.pch_header_ext.c_str(), ".h");
  STRCMP_EQUAL(toolchain_config.pch_compile_ext.c_str(), ".pch");
  STRCMP_EQUAL(toolchain_config.prefix_include_dir.c_str(), "/I");
  STRCMP_EQUAL(toolchain_config.prefix_lib_dir.c_str(), "/LIBPATH:");

  fs::path current_directory = fs::current_path();
  buildcc::ToolchainFindConfig find_config;
  find_config.env_vars.clear();
  find_config.absolute_search_paths.insert(current_directory);

  {
    CHECK_EQUAL(putenv("VSCMD_VER="), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_HOST_ARCH="), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_TGT_ARCH="), 0);
    CHECK_THROWS(std::exception, msvc.Verify(find_config));
  }

  {
    CHECK_EQUAL(putenv("VSCMD_VER="), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_HOST_ARCH=host"), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_TGT_ARCH="), 0);
    CHECK_THROWS(std::exception, msvc.Verify(find_config));
  }

  {
    CHECK_EQUAL(putenv("VSCMD_VER="), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_HOST_ARCH="), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_TGT_ARCH=target"), 0);
    CHECK_THROWS(std::exception, msvc.Verify(find_config));
  }

  {
    CHECK_EQUAL(putenv("VSCMD_VER=version"), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_HOST_ARCH="), 0);
    CHECK_EQUAL(putenv("VSCMD_ARG_TGT_ARCH="), 0);
    CHECK_THROWS(std::exception, msvc.Verify(find_config));
  }
}

TEST(ToolchainSpecializedTestGroup, Global) {
  CHECK_THROWS(std::exception, buildcc::GlobalToolchainMetadata::GetConfig(
                                   (buildcc::ToolchainId)65535));

  CHECK_THROWS(std::exception, buildcc::GlobalToolchainMetadata::GetInfoCb(
                                   (buildcc::ToolchainId)65535));

  CHECK_FALSE(buildcc::GlobalToolchainMetadata::GetInfoCb(
                  buildcc::ToolchainId::Custom)(buildcc::ToolchainExecutables())
                  .has_value());
  CHECK_FALSE(
      buildcc::GlobalToolchainMetadata::GetInfoCb(
          buildcc::ToolchainId::Undefined)(buildcc::ToolchainExecutables())
          .has_value());
}

//

void convert_executables_to_full_path(buildcc::ToolchainExecutables &exes,
                                      const std::string &ext) {
  fs::path current_path = fs::current_path().make_preferred();
  exes.assembler =
      (current_path / fmt::format("{}{}", exes.assembler, ext)).string();
  exes.c_compiler =
      (current_path / fmt::format("{}{}", exes.c_compiler, ext)).string();
  exes.cpp_compiler =
      (current_path / fmt::format("{}{}", exes.cpp_compiler, ext)).string();
  exes.archiver =
      (current_path / fmt::format("{}{}", exes.archiver, ext)).string();
  exes.linker = (current_path / fmt::format("{}{}", exes.linker, ext)).string();
}

void create_dummy_executables(const buildcc::ToolchainExecutables &exes) {
  buildcc::env::save_file(exes.assembler.c_str(), "", false);
  buildcc::env::save_file(exes.c_compiler.c_str(), "", false);
  buildcc::env::save_file(exes.cpp_compiler.c_str(), "", false);
  buildcc::env::save_file(exes.archiver.c_str(), "", false);
  buildcc::env::save_file(exes.linker.c_str(), "", false);
}

int main(int ac, char **av) {
  buildcc::env::m::VectorStringCopier copier;
  mock().installCopier(TEST_VECTOR_STRING_TYPE, copier);

  constexpr const char *const exe_ext =
      buildcc::env::get_os_executable_extension();
  std::string ext = "";
  if (exe_ext) {
    ext = exe_ext;
  }
  buildcc::ToolchainExecutables gcc_exes("as", "gcc", "g++", "ar", "ld");
  convert_executables_to_full_path(gcc_exes, ext);
  create_dummy_executables(gcc_exes);

  buildcc::ToolchainExecutables msvc_exes("cl", "cl", "cl", "lib", "link");
  convert_executables_to_full_path(msvc_exes, ext);
  create_dummy_executables(msvc_exes);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
