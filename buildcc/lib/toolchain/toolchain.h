#ifndef TARGET_INCLUDE_TOOLCHAIN_H_
#define TARGET_INCLUDE_TOOLCHAIN_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace buildcc::base {

// Base toolchain class
class Toolchain {
public:
  explicit Toolchain(std::string_view name, std::string_view asm_compiler,
                     std::string_view c_compiler, std::string_view cpp_compiler,
                     std::string_view archiver, std::string_view linker)
      : name_(name), asm_compiler_(asm_compiler), c_compiler_(c_compiler),
        cpp_compiler_(cpp_compiler), archiver_(archiver), linker_(linker) {}

  Toolchain(const Toolchain &toolchain) = delete;

  // Getters
  const std::string &GetName() const { return name_; }
  const std::string &GetAsmCompiler() const { return asm_compiler_; }
  const std::string &GetCCompiler() const { return c_compiler_; }
  const std::string &GetCppCompiler() const { return cpp_compiler_; }
  const std::string &GetArchiver() const { return archiver_; }
  const std::string &GetLinker() const { return linker_; }

private:
  std::string name_;
  std::string asm_compiler_;
  std::string c_compiler_;
  std::string cpp_compiler_;
  std::string archiver_;
  std::string linker_;
};

} // namespace buildcc::base

#endif
