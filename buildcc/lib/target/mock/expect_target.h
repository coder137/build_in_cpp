#ifndef TARGET_MOCK_EXPECT_EXPECT_TARGET_H_
#define TARGET_MOCK_EXPECT_EXPECT_TARGET_H_

#include "target.h"

namespace buildcc {

namespace internal::m {

// NOTE, No longer required
void Expect_command(unsigned int calls, bool expectation);

} // namespace internal::m

namespace base::m {

void TargetExpect_CompileTargetTask(unsigned int calls, Target *target,
                                    int compile_sources,
                                    int dummy_compile_sources);
void TargetExpect_LinkTargetTask(unsigned int calls, Target *target);

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
