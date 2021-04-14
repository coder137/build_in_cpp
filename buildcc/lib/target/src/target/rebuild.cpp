#include "target.h"

namespace buildcc::base {

// Source rechecks
void Target::SourceRemoved() {}
void Target::SourceAdded() {}
void Target::SourceUpdated() {}

// Path rechecks
void Target::PathRemoved() {}
void Target::PathAdded() {}
void Target::PathUpdated() {}

} // namespace buildcc::base
