#include "target/generator.h"

#include "expect_generator.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

static constexpr const char *const INPUT_REMOVED_FUNCTION =
    "Generator::InputRemoved";
static constexpr const char *const INPUT_ADDED_FUNCTION =
    "Generator::InputAdded";
static constexpr const char *const INPUT_UPDATED_FUNCTION =
    "Generator::InputUpdated";

static constexpr const char *const OUTPUT_CHANGED_FUNCTION =
    "Generator::OutputChanged";
static constexpr const char *const COMMAND_CHANGED_FUNCTION =
    "Generator::CommandChanged";

void Generator::InputRemoved() {
  mock().actualCall(INPUT_REMOVED_FUNCTION).onObject(this);
}
void Generator::InputAdded() {
  mock().actualCall(INPUT_ADDED_FUNCTION).onObject(this);
}
void Generator::InputUpdated() {
  mock().actualCall(INPUT_UPDATED_FUNCTION).onObject(this);
}

void Generator::OutputChanged() {
  mock().actualCall(OUTPUT_CHANGED_FUNCTION).onObject(this);
}
void Generator::CommandChanged() {
  mock().actualCall(COMMAND_CHANGED_FUNCTION).onObject(this);
}

namespace m {

void GeneratorExpect_InputRemoved(unsigned int calls, Generator *generator) {
  mock().expectNCalls(calls, INPUT_REMOVED_FUNCTION).onObject(generator);
}
void GeneratorExpect_InputAdded(unsigned int calls, Generator *generator) {
  mock().expectNCalls(calls, INPUT_ADDED_FUNCTION).onObject(generator);
}
void GeneratorExpect_InputUpdated(unsigned int calls, Generator *generator) {
  mock().expectNCalls(calls, INPUT_UPDATED_FUNCTION).onObject(generator);
}

void GeneratorExpect_OutputChanged(unsigned int calls, Generator *generator) {
  mock().expectNCalls(calls, OUTPUT_CHANGED_FUNCTION).onObject(generator);
}
void GeneratorExpect_CommandChanged(unsigned int calls, Generator *generator) {
  mock().expectNCalls(calls, COMMAND_CHANGED_FUNCTION).onObject(generator);
}

} // namespace m

} // namespace buildcc::base
