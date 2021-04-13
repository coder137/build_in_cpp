#include "target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

// Source rechecks
void Target::SourceRemoved() {
  mock().actualCall("Target::SourceRemoved").onObject(this);
}
void Target::SourceAdded() {
  mock().actualCall("Target::SourceAdded").onObject(this);
}
void Target::SourceUpdated() {
  mock().actualCall("Target::SourceUpdated").onObject(this);
}

// Path rechecks
void Target::PathRemoved() {
  mock().actualCall("Target::PathRemoved").onObject(this);
}
void Target::PathAdded() {
  mock().actualCall("Target::PathAdded").onObject(this);
}
void Target::PathUpdated() {
  mock().actualCall("Target::PathUpdated").onObject(this);
}

} // namespace buildcc::base
