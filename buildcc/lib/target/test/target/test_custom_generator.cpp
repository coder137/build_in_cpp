#include "target/custom_generator.h"

#include "expect_command.h"
#include "expect_custom_generator.h"

// #include "test_target_util.h"
// #include "taskflow/taskflow.hpp"
// #include "env/util.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(CustomGeneratorTestGroup)
{
    void teardown() {
      mock().checkExpectations();
      mock().clear();
      buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
    }
};
// clang-format on

fs::path BUILD_DIR = fs::current_path() / "intermediate" / "custom_generator";

static bool BasicGenerateCb(const buildcc::CustomGeneratorContext &ctx) {
  (void)ctx;
  return mock().actualCall("BasicGenerateCb").returnBoolValue();
}

TEST(CustomGeneratorTestGroup, Basic) {
  buildcc::CustomGenerator cgen("basic", "");
  cgen.AddGenInfo("id1", {"{gen_root_dir}/dummy_main.c"},
                  {"{gen_build_dir}/dummy_main.o"}, BasicGenerateCb);
  cgen.AddGenInfo("id2", {"{gen_root_dir}/dummy_main.cpp"}, {},
                  BasicGenerateCb);
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  buildcc::m::CustomGeneratorRunner(cgen);

  // Serialization check
  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());

    const auto &internal_map = serialization.GetLoad().internal_rels_map;
    CHECK_EQUAL(internal_map.size(), 2);
    const auto &id1_info = internal_map.at("id1");
    CHECK_EQUAL(id1_info.internal_inputs.size(), 1);
    CHECK_EQUAL(id1_info.outputs.size(), 1);

    const auto &id2_info = internal_map.at("id2");
    CHECK_EQUAL(id2_info.internal_inputs.size(), 1);
    CHECK_EQUAL(id2_info.outputs.size(), 0);
  }
}

TEST(CustomGeneratorTestGroup, Basic_Failure) {
  buildcc::CustomGenerator cgen("basic_failure", "");
  cgen.AddGenInfo("id1", {"{gen_root_dir}/dummy_main.c"}, {}, BasicGenerateCb);
  cgen.AddGenInfo("id2", {"{gen_root_dir}/dummy_main.cpp"}, {},
                  BasicGenerateCb);
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  mock().expectOneCall("BasicGenerateCb").andReturnValue(false);
  buildcc::m::CustomGeneratorRunner(cgen);

  // Load
  buildcc::internal::CustomGeneratorSerialization serialization(
      cgen.GetBinaryPath());
  CHECK_TRUE(serialization.LoadFromFile());

  const auto &id1_info = serialization.GetLoad().internal_rels_map.at("id1");
  CHECK_EQUAL(id1_info.internal_inputs.size(), 1);
  CHECK_EQUAL(id1_info.outputs.size(), 0);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);

  // Serialization check
  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());

    const auto &internal_map = serialization.GetLoad().internal_rels_map;
    CHECK_EQUAL(internal_map.size(), 1);
  }
}

TEST(CustomGeneratorTestGroup, FailureCases) {
  {
    buildcc::CustomGenerator cgen("failure_no_cb", "");
    buildcc::GenerateCb cb;
    CHECK_THROWS(std::exception, cgen.AddGenInfo("id1", {}, {}, cb));
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::CustomGenerator cgen("failure_cannot_save", "");
    fs::create_directory(
        cgen.GetBinaryPath()); // make a folder so that file cannot be saved

    cgen.AddGenInfo("id1", {}, {}, BasicGenerateCb);
    cgen.AddGenInfo("id2", {}, {}, BasicGenerateCb);
    cgen.Build();

    mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
    mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
    buildcc::m::CustomGeneratorRunner(cgen);

    CHECK_TRUE(buildcc::env::get_task_state() ==
               buildcc::env::TaskState::FAILURE);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::CustomGenerator cgen("gen_task_not_run_no_io", "");
    cgen.Build();

    buildcc::m::CustomGeneratorRunner(cgen);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

    buildcc::CustomGenerator cgen("gen_task_state_failure", "");
    cgen.AddGenInfo("id1", {}, {}, BasicGenerateCb);
    cgen.Build();
    buildcc::m::CustomGeneratorRunner(cgen);

    CHECK_TRUE(buildcc::env::get_task_state() ==
               buildcc::env::TaskState::FAILURE);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

static bool RealGenerateCb(const buildcc::CustomGeneratorContext &ctx) {
  (void)ctx;
  mock().actualCall("RealGenerateCb");
  return buildcc::env::Command::Execute("");
}

TEST(CustomGeneratorTestGroup, RealGenerate) {
  constexpr const char *const kGenName = "real_generator";
  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddGenInfo("id1", {"{gen_root_dir}/dummy_main.cpp"},
                    {"{gen_build_dir}/dummy_main.o"}, RealGenerateCb);
    cgen.AddGenInfo("id2", {"{gen_root_dir}/dummy_main.c"},
                    {"{gen_build_dir}/dummy_main.o"}, RealGenerateCb);
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_rels_map.size(), 2);

    fs::remove_all(cgen.GetBinaryPath());
  }

  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddGenInfo("id1", {"{gen_root_dir}/dummy_main.cpp"}, {},
                    RealGenerateCb);
    cgen.AddGenInfo("id2", {"{gen_root_dir}/dummy_main.c"}, {}, RealGenerateCb);
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, false);
    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    CHECK_TRUE(buildcc::env::get_task_state() ==
               buildcc::env::TaskState::FAILURE);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_rels_map.size(), 1);

    fs::remove_all(cgen.GetBinaryPath());
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::Project::Init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
