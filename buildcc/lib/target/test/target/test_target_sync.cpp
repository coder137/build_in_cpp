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

buildcc::Toolchain gcc(buildcc::Toolchain::Id::Gcc, "gcc", "as", "gcc", "g++",
                       "ar", "ldd");

TEST(TargetTestSyncGroup, CopyByConstRef) {
  buildcc::BaseTarget srcTarget("srcTarget", buildcc::TargetType::Executable,
                                gcc, "data");
  buildcc::BaseTarget destTarget("destTarget", buildcc::TargetType::Executable,
                                 gcc, "data");

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
                                 buildcc::SyncOption::SourceFiles,
                                 buildcc::SyncOption::HeaderFiles,
                                 buildcc::SyncOption::PchFiles,
                                 buildcc::SyncOption::LibDeps,
                                 buildcc::SyncOption::IncludeDirs,
                                 buildcc::SyncOption::LibDirs,
                                 buildcc::SyncOption::ExternalLibDeps,
                                 buildcc::SyncOption::PreprocessorFlags,
                                 buildcc::SyncOption::CommonCompileFlags,
                                 buildcc::SyncOption::PchCompileFlags,
                                 buildcc::SyncOption::PchObjectFlags,
                                 buildcc::SyncOption::AsmCompileFlags,
                                 buildcc::SyncOption::CCompileFlags,
                                 buildcc::SyncOption::CppCompileFlags,
                                 buildcc::SyncOption::LinkFlags,
                                 buildcc::SyncOption::CompileDependencies,
                                 buildcc::SyncOption::LinkDependencies,
                             });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, CopyByMove) {
  buildcc::BaseTarget srcTarget("srcTarget", buildcc::TargetType::Executable,
                                gcc, "data");
  buildcc::BaseTarget destTarget("destTarget", buildcc::TargetType::Executable,
                                 gcc, "data");

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
                      buildcc::SyncOption::SourceFiles,
                      buildcc::SyncOption::HeaderFiles,
                      buildcc::SyncOption::PchFiles,
                      buildcc::SyncOption::LibDeps,
                      buildcc::SyncOption::IncludeDirs,
                      buildcc::SyncOption::LibDirs,
                      buildcc::SyncOption::ExternalLibDeps,
                      buildcc::SyncOption::PreprocessorFlags,
                      buildcc::SyncOption::CommonCompileFlags,
                      buildcc::SyncOption::PchCompileFlags,
                      buildcc::SyncOption::PchObjectFlags,
                      buildcc::SyncOption::AsmCompileFlags,
                      buildcc::SyncOption::CCompileFlags,
                      buildcc::SyncOption::CppCompileFlags,
                      buildcc::SyncOption::LinkFlags,
                      buildcc::SyncOption::CompileDependencies,
                      buildcc::SyncOption::LinkDependencies,
                  });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, CopyCrash) {
  buildcc::BaseTarget srcTarget("srcTarget", buildcc::TargetType::Executable,
                                gcc, "data");
  buildcc::BaseTarget destTarget("destTarget", buildcc::TargetType::Executable,
                                 gcc, "data");

  CHECK_THROWS(std::exception,
               destTarget.Copy(srcTarget, {
                                              (buildcc::SyncOption)65535,
                                          }));
}

TEST(TargetTestSyncGroup, InsertByConstRef) {
  buildcc::BaseTarget srcTarget("srcTarget", buildcc::TargetType::Executable,
                                gcc, "data");
  buildcc::BaseTarget destTarget("destTarget", buildcc::TargetType::Executable,
                                 gcc, "data");

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

  destTarget.Insert(srcTarget, {
                                   buildcc::SyncOption::SourceFiles,
                                   buildcc::SyncOption::HeaderFiles,
                                   buildcc::SyncOption::PchFiles,
                                   buildcc::SyncOption::LibDeps,
                                   buildcc::SyncOption::IncludeDirs,
                                   buildcc::SyncOption::LibDirs,
                                   buildcc::SyncOption::ExternalLibDeps,
                                   buildcc::SyncOption::PreprocessorFlags,
                                   buildcc::SyncOption::CommonCompileFlags,
                                   buildcc::SyncOption::PchCompileFlags,
                                   buildcc::SyncOption::PchObjectFlags,
                                   buildcc::SyncOption::AsmCompileFlags,
                                   buildcc::SyncOption::CCompileFlags,
                                   buildcc::SyncOption::CppCompileFlags,
                                   buildcc::SyncOption::LinkFlags,
                                   buildcc::SyncOption::CompileDependencies,
                                   buildcc::SyncOption::LinkDependencies,
                               });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, InsertByMove) {
  buildcc::BaseTarget srcTarget("srcTarget", buildcc::TargetType::Executable,
                                gcc, "data");
  buildcc::BaseTarget destTarget("destTarget", buildcc::TargetType::Executable,
                                 gcc, "data");

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
                        buildcc::SyncOption::SourceFiles,
                        buildcc::SyncOption::HeaderFiles,
                        buildcc::SyncOption::PchFiles,
                        buildcc::SyncOption::LibDeps,
                        buildcc::SyncOption::IncludeDirs,
                        buildcc::SyncOption::LibDirs,
                        buildcc::SyncOption::ExternalLibDeps,
                        buildcc::SyncOption::PreprocessorFlags,
                        buildcc::SyncOption::CommonCompileFlags,
                        buildcc::SyncOption::PchCompileFlags,
                        buildcc::SyncOption::PchObjectFlags,
                        buildcc::SyncOption::AsmCompileFlags,
                        buildcc::SyncOption::CCompileFlags,
                        buildcc::SyncOption::CppCompileFlags,
                        buildcc::SyncOption::LinkFlags,
                        buildcc::SyncOption::CompileDependencies,
                        buildcc::SyncOption::LinkDependencies,
                    });

  CHECK_EQUAL(destTarget.GetSourceFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetHeaderFiles().size(), 1);
  CHECK_EQUAL(destTarget.GetPchFiles().size(), 1);

  CHECK_EQUAL(destTarget.GetLibDeps().size(), 1);
  CHECK_EQUAL(destTarget.GetExternalLibDeps().size(), 1);

  CHECK_EQUAL(destTarget.GetIncludeDirs().size(), 1);
  CHECK_EQUAL(destTarget.GetLibDirs().size(), 1);

  CHECK_EQUAL(destTarget.GetPreprocessorFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCommonCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetPchObjectFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetAsmCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetCppCompileFlags().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkFlags().size(), 1);

  CHECK_EQUAL(destTarget.GetCompileDependencies().size(), 1);
  CHECK_EQUAL(destTarget.GetLinkDependencies().size(), 1);
}

TEST(TargetTestSyncGroup, InsertCrash) {
  buildcc::BaseTarget srcTarget("srcTarget", buildcc::TargetType::Executable,
                                gcc, "data");
  buildcc::BaseTarget destTarget("destTarget", buildcc::TargetType::Executable,
                                 gcc, "data");

  CHECK_THROWS(std::exception,
               destTarget.Insert(srcTarget, {
                                                (buildcc::SyncOption)65535,
                                            }));
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_SYNC_INTERMEDIATE_DIR);
  fs::remove_all(buildcc::env::get_project_build_dir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
