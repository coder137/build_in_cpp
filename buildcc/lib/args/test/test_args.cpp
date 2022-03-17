#include "args/args.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(ArgsTestGroup)
{
};
// clang-format on

TEST(ArgsTestGroup, Args_BasicParse) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml"};
  int argc = av.size();

  buildcc::Args args;
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());
}

TEST(ArgsTestGroup, Args_BasicExit) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--help"};
  int argc = av.size();

  buildcc::Args args;
  CHECK_THROWS(std::exception, args.Parse(argc, av.data()));
}

TEST(ArgsTestGroup, Args_MultiToml) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--config", "configs/no_clean.toml"};
  int argc = av.size();

  buildcc::Args args;
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_FALSE(args.Clean());
}

TEST(ArgsTestGroup, Args_CustomToolchain) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--config", "configs/gcc_toolchain.toml"};
  int argc = av.size();

  buildcc::Args args;
  buildcc::ArgToolchain gcc_toolchain;
  args.AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain);
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());

  // Toolchain
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::Toolchain::Id::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.binaries.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.binaries.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.binaries.linker.c_str(), "ld");
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

  buildcc::Args args;
  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  args.AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain);
  args.AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain);
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());

  // Toolchain

  // GCC
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::Toolchain::Id::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.binaries.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.binaries.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.binaries.linker.c_str(), "ld");

  // MSVC
  CHECK_TRUE(msvc_toolchain.state.build);
  CHECK_TRUE(msvc_toolchain.state.test);
  CHECK(msvc_toolchain.id == buildcc::Toolchain::Id::Msvc);
  STRCMP_EQUAL(msvc_toolchain.name.c_str(), "msvc");
  STRCMP_EQUAL(msvc_toolchain.binaries.assembler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.binaries.c_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.binaries.cpp_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.binaries.archiver.c_str(), "lib");
  STRCMP_EQUAL(msvc_toolchain.binaries.linker.c_str(), "link");
}

TEST(ArgsTestGroup, Args_DuplicateCustomToolchain) {
  buildcc::Args args;
  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain other_gcc_toolchain;
  args.AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain);

  // CLI11 Throws an exception when multiple toolchains with same name are added
  // NOTE, This behaviour does not need to be tested since it is provided by
  // CLI11
  // This test is as an example of wrong usage by the user
  CHECK_THROWS(std::exception, args.AddToolchain("gcc", "Other gcc toolchain",
                                                 other_gcc_toolchain));
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

  buildcc::Args args;
  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgTarget gcc_target;
  args.AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain);
  args.AddTarget("gcc", "Generic gcc target", gcc_target);
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());

  // Toolchain
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::Toolchain::Id::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.binaries.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.binaries.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.binaries.linker.c_str(), "ld");

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

  buildcc::Args args;
  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgTarget gcc_target;
  args.AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain);
  args.AddTarget("gcc", "Generic gcc target", gcc_target);
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::ArgTarget msvc_target;
  args.AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain);
  args.AddTarget("msvc", "Generic msvc target", msvc_target);
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());

  // GCC

  // Toolchain
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::Toolchain::Id::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.assembler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.binaries.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.binaries.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.binaries.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.binaries.linker.c_str(), "ld");

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
  CHECK(msvc_toolchain.id == buildcc::Toolchain::Id::Msvc);
  STRCMP_EQUAL(msvc_toolchain.name.c_str(), "msvc");
  STRCMP_EQUAL(msvc_toolchain.binaries.assembler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.binaries.c_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.binaries.cpp_compiler.c_str(), "cl");
  STRCMP_EQUAL(msvc_toolchain.binaries.archiver.c_str(), "lib");
  STRCMP_EQUAL(msvc_toolchain.binaries.linker.c_str(), "link");

  // Target
  STRCMP_EQUAL(msvc_target.compile_command.c_str(),
               "{compiler} {preprocessor_flags} {include_dirs} "
               "{common_compile_flags} {compile_flags} /Fo{output} /c {input}");
  STRCMP_EQUAL(msvc_target.link_command.c_str(),
               "{linker} {link_flags} {lib_dirs} /OUT:{output} {lib_deps} "
               "{compiled_sources}");
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
