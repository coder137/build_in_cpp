/*
 * Copyright 2021 Niket Naidu. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "buildcc.h"

#include "bootstrap/buildcc_flatbuffers.h"

using namespace buildcc;

static void clean_cb();

static void schema_gen_cb(base::Generator &generator,
                          const BaseTarget &flatc_exe);

static void flatbuffers_ho_cb(TargetInfo &info);
static void cli11_ho_cb(TargetInfo &info);
static void fmt_ho_cb(TargetInfo &info);
static void spdlog_ho_cb(TargetInfo &info);
static void taskflow_ho_cb(TargetInfo &info);
static void tpl_cb(BaseTarget &target);

static void buildcc_cb(BaseTarget &target, const base::Generator &schema_gen,
                       const TargetInfo &flatbuffers_ho,
                       const TargetInfo &fmt_ho, const TargetInfo &spdlog_ho,
                       const TargetInfo &cli11_ho,
                       const TargetInfo &taskflow_ho, const BaseTarget &tpl);

static void hybrid_simple_example_cb(BaseTarget &target,
                                     const BaseTarget &libbuildcc);

int main(int argc, char **argv) noexcept {
  Args args;
  Args::ToolchainArg arg_gcc;
  args.AddToolchain("gcc", "Host GCC Toolchain", arg_gcc);
  args.Parse(argc, argv);

  Register reg(args);
  reg.Clean(clean_cb);

  Toolchain_gcc gcc;

  // Flatc Executable
  ExecutableTarget_generic flatc_exe("flatc", gcc, "third_party/flatbuffers");
  reg.Build(arg_gcc.state, flatc_cb, flatc_exe);

  // Schema
  base::Generator schema_gen("schema_gen", "buildcc/schema");
  reg.Build(schema_gen_cb, schema_gen, flatc_exe);
  reg.Dep(schema_gen, flatc_exe);

  // Flatbuffers HO lib
  TargetInfo flatbuffers_ho_lib("third_party/flatbuffers");
  reg.Callback(flatbuffers_ho_cb, flatbuffers_ho_lib);

  // CLI11 HO lib
  TargetInfo cli11_ho_lib("third_party/CLI11");
  reg.Callback(cli11_ho_cb, cli11_ho_lib);

  // fmt HO lib
  TargetInfo fmt_ho_lib("third_party/fmt");
  reg.Callback(fmt_ho_cb, fmt_ho_lib);

  // spdlog HO lib
  TargetInfo spdlog_ho_lib("third_party/spdlog");
  reg.Callback(spdlog_ho_cb, spdlog_ho_lib);

  // taskflow HO lib
  TargetInfo taskflow_ho_lib("third_party/taskflow");
  reg.Callback(taskflow_ho_cb, taskflow_ho_lib);

  // Tiny-process-library lib
  // TODO, Make this a generic selection between StaticTarget and DynamicTarget
  StaticTarget_generic tpl_lib("libtpl", gcc,
                               "third_party/tiny-process-library");
  reg.Build(arg_gcc.state, tpl_cb, tpl_lib);

  // TODO, Make this a generic selection between StaticTarget and DynamicTarget
  StaticTarget_generic buildcc_lib("libbuildcc", gcc, "buildcc");
  reg.Build(arg_gcc.state, buildcc_cb, buildcc_lib, schema_gen,
            flatbuffers_ho_lib, fmt_ho_lib, spdlog_ho_lib, cli11_ho_lib,
            taskflow_ho_lib, tpl_lib);
  reg.Dep(buildcc_lib, schema_gen);
  reg.Dep(buildcc_lib, tpl_lib);

  ExecutableTarget_generic buildcc_hybrid_simple_example(
      "buildcc_hybrid_simple_example", gcc, "example/hybrid/simple");
  reg.Build(arg_gcc.state, hybrid_simple_example_cb,
            buildcc_hybrid_simple_example, buildcc_lib);
  reg.Dep(buildcc_hybrid_simple_example, buildcc_lib);

  // reg.Test(arg_gcc.state, "{executable} --help",
  // buildcc_hybrid_simple_example,
  //          {
  //              {"example_folder",
  //               buildcc_hybrid_simple_example.GetTargetRootDir().string()},
  //          });
  reg.Test(arg_gcc.state,
           "{executable} --config {example_folder}/build_linux.toml",
           buildcc_hybrid_simple_example,
           TestConfig(
               {
                   {"example_folder",
                    buildcc_hybrid_simple_example.GetTargetRootDir().string()},
               },
               buildcc_hybrid_simple_example.GetTargetRootDir()));

  // ExecutableTarget_generic tpl_io_test("tpl_io_test", gcc,
  //                                      "third_party/tiny-process-library");
  // reg.Build(
  //     arg_gcc.state,
  //     [](BaseTarget &target, const BaseTarget &tpl_lib) {
  //       // target.AddSource("tests/io_test.cpp");
  //       target.AddSource("tests/multithread_test.cpp");

  //       for (const auto &idir : tpl_lib.GetCurrentIncludeDirs()) {
  //         target.AddIncludeDir(idir);
  //       }
  //       target.AddLibDep(tpl_lib);
  //       target.Build();
  //     },
  //     tpl_io_test, tpl_lib);
  // reg.Dep(tpl_io_test, tpl_lib);
  // reg.Test(arg_gcc.state, "{executable}", tpl_io_test);

  reg.RunBuild();

  // fs::current_path(buildcc_hybrid_simple_example.GetTargetRootDir());
  reg.RunTest();
  // fs::current_path(env::get_project_root_dir());

  // - Clang Compile Commands
  plugin::ClangCompileCommands({&buildcc_lib}).Generate();

  // - Plugin Graph
  std::string output = reg.GetTaskflow().dump();
  const bool saved = env::save_file("graph.dot", output, false);
  env::assert_fatal(saved, "Could not save graph.dot file");

  return 0;

  return 0;
}

static void clean_cb() {}

static void schema_gen_cb(base::Generator &generator,
                          const BaseTarget &flatc_exe) {
  generator.AddInput("{gen_root_dir}/path.fbs", "path_fbs");
  generator.AddInput("{gen_root_dir}/generator.fbs", "generator_fbs");
  generator.AddInput("{gen_root_dir}/target.fbs", "target_fbs");

  generator.AddOutput("{gen_build_dir}/path_generated.h");
  generator.AddOutput("{gen_build_dir}/generator_generated.h");
  generator.AddOutput("{gen_build_dir}/target_generated.h");

  generator.AddDefaultArguments({
      {"flatc_compiler", fmt::format("{}", flatc_exe.GetTargetPath())},
  });
  //   generator.AddCommand("{flatc_compiler} --help");
  generator.AddCommand(
      "{flatc_compiler} -o {gen_build_dir} -I {gen_root_dir} --gen-object-api "
      "--cpp {path_fbs} {generator_fbs} {target_fbs}");

  generator.Build();
}

static void flatbuffers_ho_cb(TargetInfo &info) {
  info.AddIncludeDir("include");
  info.GlobHeaders("include/flatbuffers");
  // TODO, Add PCH
}

static void cli11_ho_cb(TargetInfo &info) {
  info.AddIncludeDir("include");
  info.GlobHeaders("include/CLI");
  // TODO, Add PCH
}

static void fmt_ho_cb(TargetInfo &info) {
  info.AddIncludeDir("include");
  info.GlobHeaders("include/fmt");
}

static void spdlog_ho_cb(TargetInfo &info) {
  info.AddIncludeDir("include");
  info.GlobHeaders("include/spdlog");
  info.GlobHeaders("include/spdlog/cfg");
  info.GlobHeaders("include/spdlog/details");
  info.GlobHeaders("include/spdlog/fmt");
  info.GlobHeaders("include/spdlog/sinks");
}

static void taskflow_ho_cb(TargetInfo &info) {
  info.AddIncludeDir("");
  info.GlobHeaders("taskflow");
  info.GlobHeaders("taskflow/core");
  info.GlobHeaders("taskflow/core/algorithm");
  // TODO, Track more header files
}

static void tpl_cb(BaseTarget &target) {
  target.AddSource("process.cpp");
  target.AddIncludeDir("");
  target.AddHeader("process.hpp");

  // MinGW (GCC), MSVC, Clang
  if constexpr (env::is_win()) {
    target.AddSource("process_win.cpp");
    // TODO, MSVC
    // TODO, Clang
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc:
    case ToolchainId::MinGW:
      target.AddCppCompileFlag("-std=c++17");
      target.AddCppCompileFlag("-Wall");
      target.AddCppCompileFlag("-Wextra");
      break;
    default:
      break;
    }
  }

  if constexpr (env::is_linux()) {
    // TODO, GCC
    // TODO, Clang
  }

  // LOG DUMP
  for (const auto &d : target.GetCurrentSourceFiles()) {
    env::log_info(__FUNCTION__, d.string());
  }

  target.Build();
}

static void buildcc_cb(BaseTarget &target, const base::Generator &schema_gen,
                       const TargetInfo &flatbuffers_ho,
                       const TargetInfo &fmt_ho, const TargetInfo &spdlog_ho,
                       const TargetInfo &cli11_ho,
                       const TargetInfo &taskflow_ho, const BaseTarget &tpl) {
  // NOTE, Build as single lib
  target.AddIncludeDir("", true);
  const std::string &schema_build_dir =
      schema_gen.GetValueByIdentifier("gen_build_dir");
  target.AddIncludeDirAbsolute(schema_build_dir, true);

  // ENV
  target.GlobSources("lib/env/src");
  target.AddIncludeDir("lib/env/include");
  target.GlobHeaders("lib/env/include/env");

  // COMMAND
  target.GlobSources("lib/command/src");
  target.AddIncludeDir("lib/command/include");
  target.GlobHeaders("lib/command/include/command");

  // TOOLCHAIN
  target.AddIncludeDir("lib/toolchain/include");
  target.GlobHeaders("lib/toolchain/include/toolchain");

  // TARGET
  target.GlobSources("lib/target/src/common");
  target.GlobSources("lib/target/src/generator");
  target.GlobSources("lib/target/src/api");
  target.GlobSources("lib/target/src/target");
  target.GlobSources("lib/target/src/target/friend");

  target.AddIncludeDir("lib/target/include");
  target.GlobHeaders("lib/target/include/target");
  target.GlobHeaders("lib/target/include/target/api");
  target.GlobHeaders("lib/target/include/target/base");
  target.GlobHeaders("lib/target/include/target/common");
  target.GlobHeaders("lib/target/include/target/friend");
  target.GlobHeaders("lib/target/include/target/interface");
  target.GlobHeaders("lib/target/include/target/private");

  // ARGS
  target.GlobSources("lib/args/src");
  target.AddIncludeDir("lib/args/include");
  target.GlobHeaders("lib/args/include/args");

  // Specialized Toolchains
  target.AddIncludeDir("toolchains/include");
  target.GlobHeaders("toolchains/include/toolchains");

  // Specialized Targets
  target.AddIncludeDir("targets/include");
  target.GlobHeaders("targets/include/targets");

  // Plugins
  target.GlobSources("plugins/src");
  target.AddIncludeDir("plugins/include");
  target.GlobHeaders("plugins/include/plugins");

  // Third Party libraries

  const std::initializer_list<SyncOption> kInsertOptions{
      SyncOption::IncludeDirs,
      SyncOption::HeaderFiles,
  };

  // FLATBUFFERS HO
  target.Insert(flatbuffers_ho, kInsertOptions);

  // FMT HO
  target.Insert(fmt_ho, kInsertOptions);

  // SPDLOG HO
  target.Insert(spdlog_ho, kInsertOptions);

  // CLI11 HO
  target.Insert(cli11_ho, kInsertOptions);

  // TASKFLOW HO
  target.Insert(taskflow_ho, kInsertOptions);

  // TPL LIB
  target.AddLibDep(tpl);
  target.Insert(tpl, kInsertOptions);

  // TODO, ADD GCC flags
  // TODO, ADD MSVC flags
  if constexpr (env::is_win()) {
    if (target.GetToolchain().GetId() == ToolchainId::Gcc) {
      target.AddLinkFlag("-Wl,--allow-multiple-definition");
    }
  }

  switch (target.GetToolchain().GetId()) {
  case ToolchainId::Gcc: {
    target.AddPreprocessorFlag("-DFMT_HEADER_ONLY=1");
    target.AddPreprocessorFlag("-DSPDLOG_FMT_EXTERNAL");
    target.AddCppCompileFlag("-std=c++17");
    target.AddCppCompileFlag("-Wall");
    target.AddCppCompileFlag("-Wextra");
  } break;
  case ToolchainId::Msvc: {
  } break;
  default:
    break;
  }

  target.Build();
}

static void hybrid_simple_example_cb(BaseTarget &target,
                                     const BaseTarget &libbuildcc) {
  target.Insert(libbuildcc, {
                                SyncOption::PreprocessorFlags,
                                SyncOption::CppCompileFlags,
                                SyncOption::LinkFlags,
                                SyncOption::HeaderFiles,
                                SyncOption::IncludeDirs,
                                SyncOption::LibDeps,
                            });
  target.AddSource("build.cpp");
  target.AddLibDep(libbuildcc);
  target.Build();
}
