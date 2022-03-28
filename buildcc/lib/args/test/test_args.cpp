#include "args/args.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(ArgsTestGroup)
{
  void teardown() {
    buildcc::Args::Deinit();
  }
};
// clang-format on

TEST(ArgsTestGroup, Args_BasicParse) {
  UT_PRINT("Args_BasicParse\r\n");
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml"};
  int argc = av.size();

  (void)buildcc::Args::Init();
  auto &instance = buildcc::Args::Init(); // Second init does nothing when
                                          // already initialized
  instance.Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());
  buildcc::Args::Ref().name("new_name");
  STRCMP_EQUAL(buildcc::Args::Ref().get_name().c_str(), "new_name");
}

TEST(ArgsTestGroup, Args_BasicExit) {
  UT_PRINT("Args_BasicExit\r\n");
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--help"};
  int argc = av.size();

  auto &instance = buildcc::Args::Init();
  CHECK_THROWS(std::exception, instance.Parse(argc, av.data()));
}

TEST(ArgsTestGroup, Args_MultiToml) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--config", "configs/no_clean.toml"};
  int argc = av.size();

  buildcc::Args::Init().Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_FALSE(buildcc::Args::Clean());
}

TEST(ArgsTestGroup, Args_CustomToolchain) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--config", "configs/gcc_toolchain.toml"};
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Toolchain
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::ToolchainId::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.executables.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.executables.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.executables.linker.c_str(), "ld");
}

TEST(ArgsTestGroup, Args_MultipleCustomToolchain) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
      "--config",
      "configs/gcc_toolchain.toml",
      "--config",
      "configs/msvc_toolchain.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Toolchain

  // GCC
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::ToolchainId::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.executables.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.executables.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.executables.linker.c_str(), "ld");

  // MSVC
  CHECK_TRUE(msvc_toolchain.state.build);
  CHECK_TRUE(msvc_toolchain.state.test);
  CHECK(msvc_toolchain.id == buildcc::ToolchainId::Msvc);
  STRCMP_EQUAL(msvc_toolchain.name.c_str(), "msvc");
  STRCMP_EQUAL(msvc_toolchain.executables.assembler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.executables.c_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.executables.cpp_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.executables.archiver.c_str(), "lib");
  STRCMP_EQUAL(msvc_toolchain.executables.linker.c_str(), "link");
}

TEST(ArgsTestGroup, Args_DuplicateCustomToolchain) {
  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain other_gcc_toolchain;
  auto &instance = buildcc::Args::Init().AddToolchain(
      "gcc", "Generic gcc toolchain", gcc_toolchain);

  // CLI11 Throws an exception when multiple toolchains with same name are added
  // NOTE, This behaviour does not need to be tested since it is provided by
  // CLI11
  // This test is as an example of wrong usage by the user
  CHECK_THROWS(std::exception,
               (instance.AddToolchain("gcc", "Other gcc toolchain ",
                                      other_gcc_toolchain)));
}

TEST(ArgsTestGroup, Args_CustomTarget) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
      "--config",
      "configs/gcc_toolchain.toml",
      "--config",
      "configs/gcc_target.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgTarget gcc_target;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddTarget("gcc", "Generic gcc target", gcc_target)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Toolchain
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::ToolchainId::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.executables.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.executables.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.executables.linker.c_str(), "ld");

  // Target
  STRCMP_EQUAL(gcc_target.compile_command.c_str(),
               "{compiler} {preprocessor_flags} {include_dirs} "
               "{common_compile_flags} {compile_flags} -o {output} -c {input}");
  STRCMP_EQUAL(gcc_target.link_command.c_str(),
               "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
               "{lib_dirs} {lib_deps}");
}

TEST(ArgsTestGroup, Args_MultipleCustomTarget) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
      "--config",
      "configs/gcc_toolchain.toml",
      "--config",
      "configs/gcc_target.toml",
      "--config",
      "configs/msvc_toolchain.toml",
      "--config",
      "configs/msvc_target.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgTarget gcc_target;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::ArgTarget msvc_target;

  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddTarget("gcc", "Generic gcc target", gcc_target)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .AddTarget("msvc", "Generic msvc target", msvc_target)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // GCC

  // Toolchain
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::ToolchainId::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.executables.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.executables.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.executables.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.executables.linker.c_str(), "ld");

  // Target
  STRCMP_EQUAL(gcc_target.compile_command.c_str(),
               "{compiler} {preprocessor_flags} {include_dirs} "
               "{common_compile_flags} {compile_flags} -o {output} -c {input}");
  STRCMP_EQUAL(gcc_target.link_command.c_str(),
               "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
               "{lib_dirs} {lib_deps}");

  // MSVC

  // Toolchain
  CHECK_TRUE(msvc_toolchain.state.build);
  CHECK_TRUE(msvc_toolchain.state.test);
  CHECK(msvc_toolchain.id == buildcc::ToolchainId::Msvc);
  STRCMP_EQUAL(msvc_toolchain.name.c_str(), "msvc");
  STRCMP_EQUAL(msvc_toolchain.executables.assembler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.executables.c_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.executables.cpp_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.executables.archiver.c_str(), "lib");
  STRCMP_EQUAL(msvc_toolchain.executables.linker.c_str(), "link");

  // Target
  STRCMP_EQUAL(msvc_target.compile_command.c_str(),
               "{compiler} {preprocessor_flags} {include_dirs} "
               "{common_compile_flags} {compile_flags} /Fo{output} /c {input}");
  STRCMP_EQUAL(msvc_target.link_command.c_str(),
               "{linker} {link_flags} {lib_dirs} /OUT:{output} {lib_deps} "
               "{compiled_sources}");
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::destroyGlobalDetector();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
