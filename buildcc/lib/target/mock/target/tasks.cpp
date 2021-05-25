#include "target.h"

#include "CppUTestExt/MockSupport.h"

namespace buildcc::base {

static constexpr const char *const COMPILETARGETTASK_FUNCTION =
    "Target::CompileTargetTask";
static constexpr const char *const COMPILETARGETTASK_COMPILESOURCES_PARAM =
    "compile_sources";
static constexpr const char *const COMPILETARGETTASK_DUMMYCOMPILESOURCES_PARAM =
    "dummy_compile_sources";

static constexpr const char *const LINKTARGETTASK_FUNCTION =
    "Target::LinkTargetTask";

void Target::CompileTargetTask(
    const std::vector<fs::path> &&compile_sources,
    const std::vector<fs::path> &&dummy_compile_sources) {
  mock()
      .actualCall(COMPILETARGETTASK_FUNCTION)
      .onObject(this)
      .withIntParameter(COMPILETARGETTASK_COMPILESOURCES_PARAM,
                        compile_sources.size())
      .withIntParameter(COMPILETARGETTASK_DUMMYCOMPILESOURCES_PARAM,
                        dummy_compile_sources.size());
}

void Target::LinkTargetTask() {
  mock().actualCall(LINKTARGETTASK_FUNCTION).onObject(this);
}

namespace m {

void TargetExpect_CompileTargetTask(unsigned int calls, Target *target,
                                    int compile_sources,
                                    int dummy_compile_sources) {
  mock()
      .expectNCalls(calls, COMPILETARGETTASK_FUNCTION)
      .onObject(target)
      .withIntParameter(COMPILETARGETTASK_COMPILESOURCES_PARAM, compile_sources)
      .withIntParameter(COMPILETARGETTASK_DUMMYCOMPILESOURCES_PARAM,
                        dummy_compile_sources);
}

void TargetExpect_LinkTargetTask(unsigned int calls, Target *target) {
  mock().expectNCalls(calls, LINKTARGETTASK_FUNCTION).onObject(target);
}

} // namespace m

} // namespace buildcc::base
