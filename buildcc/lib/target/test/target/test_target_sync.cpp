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
TEST_GROUP(TargetTestSyncGroup)
{
    void teardown() {
    }
};
// clang-format on

buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                             "gcc", "g++", "ar", "ldd");

TEST(TargetTestSyncGroup, CopyByConstRef) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::TargetType::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::TargetType::Executable, gcc, "data");

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

  destTarget.Copy(srcTarget, {
                                 buildcc::base::SyncOption::SourceFiles,
                                 buildcc::base::SyncOption::HeaderFiles,
                                 buildcc::base::SyncOption::PchFiles,
                                 buildcc::base::SyncOption::LibDeps,
                                 buildcc::base::SyncOption::IncludeDirs,
                                 buildcc::base::SyncOption::LibDirs,
                                 buildcc::base::SyncOption::ExternalLibDeps,
                                 buildcc::base::SyncOption::PreprocessorFlags,
                                 buildcc::base::SyncOption::CommonCompileFlags,
                                 buildcc::base::SyncOption::PchCompileFlags,
                                 buildcc::base::SyncOption::PchObjectFlags,
                                 buildcc::base::SyncOption::AsmCompileFlags,
                                 buildcc::base::SyncOption::CCompileFlags,
                                 buildcc::base::SyncOption::CppCompileFlags,
                                 buildcc::base::SyncOption::LinkFlags,
                                 buildcc::base::SyncOption::CompileDependencies,
                                 buildcc::base::SyncOption::LinkDependencies,
                             });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetTargetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, CopyByMove) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::TargetType::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::TargetType::Executable, gcc, "data");

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

  destTarget.Copy(std::move(srcTarget),
                  {
                      buildcc::base::SyncOption::SourceFiles,
                      buildcc::base::SyncOption::HeaderFiles,
                      buildcc::base::SyncOption::PchFiles,
                      buildcc::base::SyncOption::LibDeps,
                      buildcc::base::SyncOption::IncludeDirs,
                      buildcc::base::SyncOption::LibDirs,
                      buildcc::base::SyncOption::ExternalLibDeps,
                      buildcc::base::SyncOption::PreprocessorFlags,
                      buildcc::base::SyncOption::CommonCompileFlags,
                      buildcc::base::SyncOption::PchCompileFlags,
                      buildcc::base::SyncOption::PchObjectFlags,
                      buildcc::base::SyncOption::AsmCompileFlags,
                      buildcc::base::SyncOption::CCompileFlags,
                      buildcc::base::SyncOption::CppCompileFlags,
                      buildcc::base::SyncOption::LinkFlags,
                      buildcc::base::SyncOption::CompileDependencies,
                      buildcc::base::SyncOption::LinkDependencies,
                  });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetTargetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, CopyCrash) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::TargetType::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::TargetType::Executable, gcc, "data");

  CHECK_THROWS(std::exception,
               destTarget.Copy(srcTarget, {
                                              (buildcc::base::SyncOption)65535,
                                          }));
}

TEST(TargetTestSyncGroup, InsertByConstRef) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::TargetType::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::TargetType::Executable, gcc, "data");

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

  destTarget.Insert(srcTarget,
                    {
                        buildcc::base::SyncOption::SourceFiles,
                        buildcc::base::SyncOption::HeaderFiles,
                        buildcc::base::SyncOption::PchFiles,
                        buildcc::base::SyncOption::LibDeps,
                        buildcc::base::SyncOption::IncludeDirs,
                        buildcc::base::SyncOption::LibDirs,
                        buildcc::base::SyncOption::ExternalLibDeps,
                        buildcc::base::SyncOption::PreprocessorFlags,
                        buildcc::base::SyncOption::CommonCompileFlags,
                        buildcc::base::SyncOption::PchCompileFlags,
                        buildcc::base::SyncOption::PchObjectFlags,
                        buildcc::base::SyncOption::AsmCompileFlags,
                        buildcc::base::SyncOption::CCompileFlags,
                        buildcc::base::SyncOption::CppCompileFlags,
                        buildcc::base::SyncOption::LinkFlags,
                        buildcc::base::SyncOption::CompileDependencies,
                        buildcc::base::SyncOption::LinkDependencies,
                    });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetTargetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, InsertByMove) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::TargetType::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::TargetType::Executable, gcc, "data");

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

  destTarget.Insert(std::move(srcTarget),
                    {
                        buildcc::base::SyncOption::SourceFiles,
                        buildcc::base::SyncOption::HeaderFiles,
                        buildcc::base::SyncOption::PchFiles,
                        buildcc::base::SyncOption::LibDeps,
                        buildcc::base::SyncOption::IncludeDirs,
                        buildcc::base::SyncOption::LibDirs,
                        buildcc::base::SyncOption::ExternalLibDeps,
                        buildcc::base::SyncOption::PreprocessorFlags,
                        buildcc::base::SyncOption::CommonCompileFlags,
                        buildcc::base::SyncOption::PchCompileFlags,
                        buildcc::base::SyncOption::PchObjectFlags,
                        buildcc::base::SyncOption::AsmCompileFlags,
                        buildcc::base::SyncOption::CCompileFlags,
                        buildcc::base::SyncOption::CppCompileFlags,
                        buildcc::base::SyncOption::LinkFlags,
                        buildcc::base::SyncOption::CompileDependencies,
                        buildcc::base::SyncOption::LinkDependencies,
                    });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetTargetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCurrentCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetCurrentLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, InsertCrash) {
  buildcc::base::Target srcTarget(
      "srcTarget", buildcc::base::TargetType::Executable, gcc, "data");
  buildcc::base::Target destTarget(
      "destTarget", buildcc::base::TargetType::Executable, gcc, "data");

  CHECK_THROWS(
      std::exception,
      destTarget.Insert(srcTarget, {
                                       (buildcc::base::SyncOption)65535,
                                   }));
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_SYNC_INTERMEDIATE_DIR);
  fs::remove_all(buildcc::env::get_project_build_dir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
