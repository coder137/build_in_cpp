#include "target/friend/file_extension.h"
#include "target/target.h"

#include "env/env.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include <unordered_map>

// clang-format off
TEST_GROUP(TargetFileExtensionTestGroup)
{
};
// clang-format on

static buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc",
                                    "as", "gcc", "g++", "ar", "ld");

TEST(TargetFileExtensionTestGroup, GetType) {
  constexpr const char *const NAME = "GetType.exe";
  buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");

  // Use config defaults and test
  buildcc::base::FileExt ext(target);
  buildcc::base::FileExt::Type type;

  //  C
  type = ext.GetType(fs::current_path() / "my_filepath.c");
  CHECK(type == buildcc::base::FileExt::Type::C);
  // Cpp
  type = ext.GetType(fs::current_path() / "my_filepath.cpp");
  CHECK(type == buildcc::base::FileExt::Type::Cpp);

  type = ext.GetType(fs::current_path() / "my_filepath.cc");
  CHECK(type == buildcc::base::FileExt::Type::Cpp);

  type = ext.GetType(fs::current_path() / "my_filepath.cxx");
  CHECK(type == buildcc::base::FileExt::Type::Cpp);

  // Asm
  type = ext.GetType(fs::current_path() / "my_filepath.asm");
  CHECK(type == buildcc::base::FileExt::Type::Asm);

  type = ext.GetType(fs::current_path() / "my_filepath.s");
  CHECK(type == buildcc::base::FileExt::Type::Asm);

  type = ext.GetType(fs::current_path() / "my_filepath.S");
  CHECK(type == buildcc::base::FileExt::Type::Asm);

  // Header
  type = ext.GetType(fs::current_path() / "my_filepath.h");
  CHECK(type == buildcc::base::FileExt::Type::Header);

  type = ext.GetType(fs::current_path() / "my_filepath.hpp");
  CHECK(type == buildcc::base::FileExt::Type::Header);

  // Invalid
  type = ext.GetType(fs::current_path() / "my_filepath.invalid");
  CHECK(type == buildcc::base::FileExt::Type::Invalid);

  type = ext.GetType(fs::current_path() / "my_filepath.notfound");
  CHECK(type == buildcc::base::FileExt::Type::Invalid);

  type = ext.GetType(fs::current_path());
  CHECK(type == buildcc::base::FileExt::Type::Invalid);
}

TEST(TargetFileExtensionTestGroup, GetCompiler) {
  std::string compiler;

  compiler = buildcc::base::FileExt::GetCompiler(
                 buildcc::base::FileExt::Type::Asm, gcc)
                 .value();
  STRCMP_EQUAL(compiler.c_str(), "as");

  compiler =
      buildcc::base::FileExt::GetCompiler(buildcc::base::FileExt::Type::C, gcc)
          .value();
  STRCMP_EQUAL(compiler.c_str(), "gcc");

  compiler = buildcc::base::FileExt::GetCompiler(
                 buildcc::base::FileExt::Type::Cpp, gcc)
                 .value();
  STRCMP_EQUAL(compiler.c_str(), "g++");

  CHECK_THROWS(std::exception, buildcc::base::FileExt::GetCompiler(
                                   buildcc::base::FileExt::Type::Header, gcc)
                                   .value());

  CHECK_THROWS(std::exception, buildcc::base::FileExt::GetCompiler(
                                   buildcc::base::FileExt::Type::Invalid, gcc)
                                   .value());
}

TEST(TargetFileExtensionTestGroup, GetCompileFlags) {
  const std::unordered_map<buildcc::base::FileExt::Type, std::string>
      relational_data{
          {buildcc::base::FileExt::Type::Asm, "ASM"},
          {buildcc::base::FileExt::Type::C, "C"},
          {buildcc::base::FileExt::Type::Cpp, "CPP"},
      };

  buildcc::base::FileExt::GetCompileFlags(buildcc::base::FileExt::Type::Asm,
                                          relational_data);

  CHECK_THROWS(std::exception,
               buildcc::base::FileExt::GetCompileFlags(
                   buildcc::base::FileExt::Type::Header, relational_data)
                   .value());

  CHECK_THROWS(std::exception,
               buildcc::base::FileExt::GetCompileFlags(
                   buildcc::base::FileExt::Type::Invalid, relational_data)
                   .value());
}

TEST(TargetFileExtensionTestGroup, SetSourceState) {
  constexpr const char *const NAME = "SetSourceState.exe";
  buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");

  // Use config defaults and test
  buildcc::base::FileExt ext(target);

  CHECK_FALSE(target.GetState().contains_asm);
  CHECK_FALSE(target.GetState().contains_c);
  CHECK_FALSE(target.GetState().contains_cpp);

  ext.SetSourceState(buildcc::base::FileExt::Type::Invalid);
  ext.SetSourceState(buildcc::base::FileExt::Type::Header);
  CHECK_FALSE(target.GetState().contains_asm);
  CHECK_FALSE(target.GetState().contains_c);
  CHECK_FALSE(target.GetState().contains_cpp);

  ext.SetSourceState(buildcc::base::FileExt::Type::Asm);
  CHECK_TRUE(target.GetState().contains_asm);
  CHECK_FALSE(target.GetState().contains_c);
  CHECK_FALSE(target.GetState().contains_cpp);

  ext.SetSourceState(buildcc::base::FileExt::Type::C);
  CHECK_TRUE(target.GetState().contains_asm);
  CHECK_TRUE(target.GetState().contains_c);
  CHECK_FALSE(target.GetState().contains_cpp);

  ext.SetSourceState(buildcc::base::FileExt::Type::Cpp);
  CHECK_TRUE(target.GetState().contains_asm);
  CHECK_TRUE(target.GetState().contains_c);
  CHECK_TRUE(target.GetState().contains_cpp);
}

int main(int ac, char **av) {
  buildcc::env::init(fs::current_path(), fs::current_path() / "intermediate" /
                                             "target_file_extension");
  fs::remove_all(buildcc::env::get_project_build_dir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
