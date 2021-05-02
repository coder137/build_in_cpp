// NOTE, This source file is only present so that `mock/recheck_states.cpp` can
// be used accurately
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

void Target::DirChanged() {}
void Target::FlagChanged() {}
void Target::ExternalLibChanged() {}

} // namespace buildcc::base
