#ifndef TARGET_INCLUDE_TOOLCHAIN_H_
#define TARGET_INCLUDE_TOOLCHAIN_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace buildcc::base {

// Base toolchain class
class Toolchain {
public:
  explicit Toolchain(const std::string &name, const std::string &asm_compiler,
                     const std::string &c_compiler,
                     const std::string &cpp_compiler,
                     const std::string &static_lib_compiler,
                     const std::string &dynamic_lib_compiler)
      : name_(name), asm_compiler_(asm_compiler), c_compiler_(c_compiler),
        cpp_compiler_(cpp_compiler), static_lib_compiler_(static_lib_compiler),
        dynamic_lib_compiler_(dynamic_lib_compiler) {}

  // Setters
  bool AddExecutable(std::string name, std::string executable);

  // TODO, Add Append equivalent
  void AddPreprocessorFlag(const std::string &preprocessor_flag);
  void AddCCompileFlag(const std::string &c_compile_flag);
  void AddCppCompileFlag(const std::string &cpp_compile_flag);
  void AddLinkFlag(const std::string &link_flag);

  // Getters
  const std::string &GetName() const { return name_; }
  const std::string &GetAsmCompiler() const { return asm_compiler_; }
  const std::string &GetCCompiler() const { return c_compiler_; }
  const std::string &GetCppCompiler() const { return cpp_compiler_; }
  const std::string &GetStaticLibCompiler() const {
    return static_lib_compiler_;
  }
  const std::string &GetDynamicLibCompiler() const {
    return dynamic_lib_compiler_;
  }
  const std::string &GetExecutable(const std::string &name) const {
    return executables_.at(name);
  }

private:
  std::string name_;
  std::string asm_compiler_;
  std::string c_compiler_;
  std::string cpp_compiler_;
  std::string static_lib_compiler_;
  std::string dynamic_lib_compiler_;

  std::unordered_map<std::string, std::string> executables_;
};

} // namespace buildcc::base

#endif
