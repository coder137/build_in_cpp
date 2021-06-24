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

#ifndef TARGETS_TARGET_CONSTANTS_H_
#define TARGETS_TARGET_CONSTANTS_H_

#include <string_view>

namespace buildcc {

// Extensions
constexpr std::string_view kGccStaticLibExt = ".a";
constexpr std::string_view kGccDynamicLibExt = ".so";
constexpr std::string_view kWinExecutableExt = ".exe";
constexpr std::string_view kWinStaticLibExt = ".lib";
// Why is `kWinDynamicLibExt != .dll` but `.lib` instead?
// See `kMsvcDynamicLibLinkCommand` in `target_command.h`
// IMPLIB .lib stubs are what is linked during link time
// OUT .dll needs to be present in the executable folder during runtime
constexpr std::string_view kWinDynamicLibExt = ".lib";

// GCC
constexpr std::string_view kGccPrefixIncludeDir = "-I";
constexpr std::string_view kGccPrefixLibDir = "-L";
constexpr std::string_view kGccGenericCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} -o "
    "{output} -c {input}";
constexpr std::string_view kGccExecutableLinkCommand =
    "{cpp_compiler} {link_flags} {compiled_sources} -o {output} "
    "{lib_dirs} {lib_deps}";
constexpr std::string_view kGccStaticLibLinkCommand =
    "{archiver} rcs {output} {compiled_sources}";
constexpr std::string_view kGccDynamicLibCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} "
    "-fpic -o {output} -c {input}";
constexpr std::string_view kGccDynamicLibLinkCommand =
    "{cpp_compiler} -shared {link_flags} {compiled_sources} -o {output}";

// MSVC
constexpr std::string_view kMsvcPrefixIncludeDir = "/I";
constexpr std::string_view kMsvcPrefixLibDir = "/LIBPATH:";
// TODO, Split this into individual CompileCommands if any differences occur
constexpr std::string_view kMsvcCompileCommand =
    "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} "
    "/Fo{output} /c {input}";
constexpr std::string_view kMsvcExecutableLinkCommand =
    "{linker} {link_flags} {lib_dirs} /OUT:{output} {lib_deps} "
    "{compiled_sources}";
constexpr std::string_view kMsvcStaticLibLinkCommand =
    "{archiver} {link_flags} /OUT:{output} {compiled_sources}";
constexpr std::string_view kMsvcDynamicLibLinkCommand =
    "{linker} /DLL {link_flags} /OUT:{output}.dll /IMPLIB:{output} "
    "{compiled_sources}";

} // namespace buildcc

#endif
