#ifndef BUILDCC_INCLUDE_TOOLCHAIN_H_
#define BUILDCC_INCLUDE_TOOLCHAIN_H_

#include <string>
#include <vector>

namespace buildcc {

// Base toolchain class
class Toolchain {
public:
  explicit Toolchain(const std::string &name, const std::string &c_compiler,
                     const std::string &cpp_compiler)
      : name_(name), c_compiler_(c_compiler), cpp_compiler_(cpp_compiler) {}

  // Setters
  // TODO, Add an Append equivalent for the same
  void AddPreprocessorFlag() {}
  void AppendPreprocessorFlags() {}

  void AddCCompileFlag() {}
  void AppendCCompileFlags() {}

  void AddCppCompileFlag() {}
  void AppendCppCompileFlags() {}

  void AddLinkFlag() {}
  void AppendLinkFlags() {}

  // Getters
  const std::string &GetName() const { return name_; }
  const std::string &GetCCompiler() const { return c_compiler_; }
  const std::string &GetCppCompiler() const { return cpp_compiler_; }

  const std::vector<std::string> &GetPreprocessorFlags() const {
    return preprocessor_flags_;
  }
  const std::vector<std::string> &GetCCompileFlags() const {
    return c_compile_flags_;
  }
  const std::vector<std::string> &GetCppCompileFlags() const {
    return cpp_compile_flags_;
  }
  const std::vector<std::string> &GetLinkFlags() const { return link_flags_; }

private:
  std::string name_;
  std::string c_compiler_;
  std::string cpp_compiler_;

  std::vector<std::string> preprocessor_flags_;
  std::vector<std::string> c_compile_flags_;
  std::vector<std::string> cpp_compile_flags_;
  std::vector<std::string> link_flags_;
};

// Overrides for certain systems
class Toolchain_Gcc : public Toolchain {
  explicit Toolchain_Gcc(const std::string &name, const std::string &c_compiler,
                         const std::string &cpp_compiler)
      : Toolchain(name, c_compiler, cpp_compiler) {
    // TODO, Add some flags by default here
  }
};

} // namespace buildcc

#endif
