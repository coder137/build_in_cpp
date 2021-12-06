#ifndef COMMAND_MOCK_MOCK_COMMAND_COPIER_H_
#define COMMAND_MOCK_MOCK_COMMAND_COPIER_H_

#include <string>
#include <vector>

#include "CppUTestExt/MockSupport.h"

namespace buildcc::m {

class VectorStringCopier : public MockNamedValueCopier {
public:
  void copy(void *out, const void *in) override {
    const std::vector<std::string> *vin = (const std::vector<std::string> *)in;
    std::vector<std::string> *vout = (std::vector<std::string> *)out;

    if (vout == nullptr || vin == nullptr) {
      return;
    }

    vout->clear();
    vout->insert(vout->end(), vin->begin(), vin->end());
  }
};

inline void InstallVectorStringCopier() {
  VectorStringCopier copier;
  mock().installCopier("vector_string", copier);
}

} // namespace buildcc::m

#endif
