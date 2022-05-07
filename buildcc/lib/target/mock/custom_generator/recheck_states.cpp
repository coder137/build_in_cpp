#include "target/custom_generator.h"

#include "expect_custom_generator.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc {

static constexpr const char *const ID_REMOVED_FUNCTION =
    "CustomGenerator::IdRemoved";
static constexpr const char *const ID_ADDED_FUNCTION =
    "CustomGenerator::IdAdded";
static constexpr const char *const ID_UPDATED_FUNCTION =
    "CustomGenerator::IdUpdated";

void CustomGenerator::IdRemoved() {
  mock().actualCall(ID_REMOVED_FUNCTION).onObject(this);
}
void CustomGenerator::IdAdded() {
  mock().actualCall(ID_ADDED_FUNCTION).onObject(this);
}
void CustomGenerator::IdUpdated() {
  mock().actualCall(ID_UPDATED_FUNCTION).onObject(this);
}

namespace m {

void GeneratorExpect_IdRemoved(unsigned int calls, CustomGenerator *generator) {
  mock().expectNCalls(calls, ID_REMOVED_FUNCTION).onObject(generator);
}
void GeneratorExpect_IdAdded(unsigned int calls, CustomGenerator *generator) {
  mock().expectNCalls(calls, ID_ADDED_FUNCTION).onObject(generator);
}
void GeneratorExpect_IdUpdated(unsigned int calls, CustomGenerator *generator) {
  mock().expectNCalls(calls, ID_UPDATED_FUNCTION).onObject(generator);
}

} // namespace m

} // namespace buildcc
