#ifndef TARGET_MOCK_EXPECT_TARGET_H_
#define TARGET_MOCK_EXPECT_TARGET_H_

#include "target/target.h"

namespace buildcc {

namespace base::m {

void TargetExpect_SourceRemoved(unsigned int calls, Target *target);
void TargetExpect_SourceAdded(unsigned int calls, Target *target);
void TargetExpect_SourceUpdated(unsigned int calls, Target *target);

void TargetExpect_PathRemoved(unsigned int calls, Target *target);
void TargetExpect_PathAdded(unsigned int calls, Target *target);
void TargetExpect_PathUpdated(unsigned int calls, Target *target);

void TargetExpect_DirChanged(unsigned int calls, Target *target);
void TargetExpect_FlagChanged(unsigned int calls, Target *target);
void TargetExpect_ExternalLibChanged(unsigned int calls, Target *target);

} // namespace base::m

} // namespace buildcc

#endif
