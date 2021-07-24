#include "target/target.h"

#include "expect_target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

static constexpr const char *const SOURCE_REMOVED_FUNCTION =
    "Target::SourceRemoved";
static constexpr const char *const SOURCE_ADDED_FUNCTION =
    "Target::SourceAdded";
static constexpr const char *const SOURCE_UPDATED_FUNCTION =
    "Target::SourceUpdated";

static constexpr const char *const PATH_REMOVED_FUNCTION =
    "Target::PathRemoved";
static constexpr const char *const PATH_ADDED_FUNCTION = "Target::PathAdded";
static constexpr const char *const PATH_UPDATED_FUNCTION =
    "Target::PathUpdated";

static constexpr const char *const DIR_CHANGED_FUNCTION = "Target::DirChanged";
static constexpr const char *const FLAG_CHANGED_FUNCTION =
    "Target::FlagChanged";
static constexpr const char *const EXTERNAL_LIB_CHANGED_FUNCTION =
    "Target::ExternalLibChanged";

// Source rechecks
void Target::SourceRemoved() {
  mock().actualCall(SOURCE_REMOVED_FUNCTION).onObject(this);
}
void Target::SourceAdded() {
  mock().actualCall(SOURCE_ADDED_FUNCTION).onObject(this);
}
void Target::SourceUpdated() {
  mock().actualCall(SOURCE_UPDATED_FUNCTION).onObject(this);
}

// Path rechecks
void Target::PathRemoved() {
  mock().actualCall(PATH_REMOVED_FUNCTION).onObject(this);
}
void Target::PathAdded() {
  mock().actualCall(PATH_ADDED_FUNCTION).onObject(this);
}
void Target::PathUpdated() {
  mock().actualCall(PATH_UPDATED_FUNCTION).onObject(this);
}

void Target::DirChanged() {
  mock().actualCall(DIR_CHANGED_FUNCTION).onObject(this);
}

void Target::FlagChanged() {
  mock().actualCall(FLAG_CHANGED_FUNCTION).onObject(this);
}

void Target::ExternalLibChanged() {
  mock().actualCall(EXTERNAL_LIB_CHANGED_FUNCTION).onObject(this);
}

namespace m {

void TargetExpect_SourceRemoved(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, SOURCE_REMOVED_FUNCTION).onObject(target);
}
void TargetExpect_SourceAdded(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, SOURCE_ADDED_FUNCTION).onObject(target);
}
void TargetExpect_SourceUpdated(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, SOURCE_UPDATED_FUNCTION).onObject(target);
}

void TargetExpect_PathRemoved(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, PATH_REMOVED_FUNCTION).onObject(target);
}
void TargetExpect_PathAdded(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, PATH_ADDED_FUNCTION).onObject(target);
}
void TargetExpect_PathUpdated(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, PATH_UPDATED_FUNCTION).onObject(target);
}

void TargetExpect_DirChanged(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, DIR_CHANGED_FUNCTION).onObject(target);
}

void TargetExpect_FlagChanged(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, FLAG_CHANGED_FUNCTION).onObject(target);
}

void TargetExpect_ExternalLibChanged(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, EXTERNAL_LIB_CHANGED_FUNCTION).onObject(target);
}

} // namespace m

} // namespace buildcc::base
