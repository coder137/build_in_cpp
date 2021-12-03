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

#include "bootstrap/build_buildcc.h"

namespace buildcc {

void schema_gen_cb(BaseGenerator &generator, const BaseTarget &flatc_exe) {
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

void buildcc_cb(BaseTarget &target, const BaseGenerator &schema_gen,
                const TargetInfo &flatbuffers_ho, const TargetInfo &fmt_ho,
                const TargetInfo &spdlog_ho, const TargetInfo &cli11_ho,
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

  if constexpr (env::is_win()) {
    // TODO, Clang
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc:
    case ToolchainId::MinGW: {
      target.AddPreprocessorFlag("-DFMT_HEADER_ONLY=1");
      target.AddPreprocessorFlag("-DSPDLOG_FMT_EXTERNAL");
      // For MINGW
      target.AddLinkFlag("-Wl,--allow-multiple-definition");
    } break;
    case ToolchainId::Msvc: {
      target.AddPreprocessorFlag("/DFMT_HEADER_ONLY=1");
      target.AddPreprocessorFlag("/DSPDLOG_FMT_EXTERNAL");
    } break;
    default:
      break;
    }
  }

  if constexpr (env::is_linux()) {
    // TODO, Clang
    switch (target.GetToolchain().GetId()) {
    case ToolchainId::Gcc: {
      target.AddPreprocessorFlag("-DFMT_HEADER_ONLY=1");
      target.AddPreprocessorFlag("-DSPDLOG_FMT_EXTERNAL");
      target.AddLibDep("-lpthread");
    } break;
    default:
      break;
    }
  }

  // TODO, Other OS's

  target.Build();
}

} // namespace buildcc
