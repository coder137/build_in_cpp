#include "constants.h"

#include "target/target.h"

#include "env/env.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestCopyGroup)
{
    void teardown() {
    }
};
// clang-format on

buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                             "gcc", "g++", "ar", "ldd");

TEST(TargetTestCopyGroup, CopyAll) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::Target::Type::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::Target::Type::Executable, gcc, "data");

  srcTarget.AddSource("dummy_main.c");
  srcTarget.AddIncludeDir("include", true);
  srcTarget.AddPch("include/include_header.h");
  srcTarget.AddLibDep(srcTarget);
  srcTarget.AddLibDep("testLib.a");
  srcTarget.AddLibDir("include");

  srcTarget.AddPreprocessorFlag("-DTEST");
  srcTarget.AddCommonCompileFlag("-O0");
  srcTarget.AddPchCompileFlag("-pch_compile");
  srcTarget.AddPchObjectFlag("-pch_object");
  srcTarget.AddAsmCompileFlag("-march=test");
  srcTarget.AddCCompileFlag("-std=c11");
  srcTarget.AddCppCompileFlag("-std=c++17");
  srcTarget.AddLinkFlag("-nostdinc");
  srcTarget.AddCompileDependency("new_source.cpp");
  srcTarget.AddLinkDependency("new_source.cpp");

  destTarget.Copy(srcTarget,
                  {
                      buildcc::base::Target::CopyOption::SourceFiles,
                      buildcc::base::Target::CopyOption::HeaderFiles,
                      buildcc::base::Target::CopyOption::PchFiles,
                      buildcc::base::Target::CopyOption::LibDeps,
                      buildcc::base::Target::CopyOption::IncludeDirs,
                      buildcc::base::Target::CopyOption::LibDirs,
                      buildcc::base::Target::CopyOption::ExternalLibDeps,
                      buildcc::base::Target::CopyOption::PreprocessorFlags,
                      buildcc::base::Target::CopyOption::CommonCompileFlags,
                      buildcc::base::Target::CopyOption::PchCompileFlags,
                      buildcc::base::Target::CopyOption::PchObjectFlags,
                      buildcc::base::Target::CopyOption::AsmCompileFlags,
                      buildcc::base::Target::CopyOption::CCompileFlags,
                      buildcc::base::Target::CopyOption::CppCompileFlags,
                      buildcc::base::Target::CopyOption::LinkFlags,
                      buildcc::base::Target::CopyOption::CompileDependencies,
                      buildcc::base::Target::CopyOption::LinkDependencies,
                  });
}

TEST(TargetTestCopyGroup, CopyCrash) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::Target::Type::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::Target::Type::Executable, gcc, "data");

  CHECK_THROWS(
      std::exception,
      destTarget.Copy(srcTarget, {
                                     (buildcc::base::Target::CopyOption)65535,
                                 }));
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_COPY_INTERMEDIATE_DIR);
  fs::remove_all(buildcc::env::get_project_build_dir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
