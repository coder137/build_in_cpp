#include "target/generator.h"

#include "expect_command.h"
#include "expect_generator.h"

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
    }
};
// clang-format on

fs::path BUILD_DIR = fs::current_path() / "intermediate" / "custom_generator";

static std::unordered_map<std::string, tf::Task>
BasicGenerateCb(tf::Subflow &subflow, buildcc::CustomGeneratorContext &ctx) {
  mock().actualCall("BasicGenerateCb");
  std::unordered_map<std::string, tf::Task> uom;
  for (const auto &miter : ctx.selected_schema) {
    mock().actualCall(miter.first.c_str());
    auto task = subflow.placeholder();
    uom.emplace(miter.first, task);
  }
  return uom;
}

TEST(CustomGeneratorTestGroup, Basic) {
  buildcc::CustomGenerator cgen("basic", "");
  cgen.AddGenerateCb(BasicGenerateCb);
  cgen.AddRelInputOutput("id1", {"{gen_root_dir}/dummy_main.c"}, {});
  cgen.AddRelInputOutput("id2", {"{gen_root_dir}/dummy_main.cpp"}, {});
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb");
  mock().expectOneCall("id1");
  mock().expectOneCall("id2");
  buildcc::m::CustomGeneratorRunner(cgen);
}

// tf::Task should not be empty
static std::unordered_map<std::string, tf::Task>
BadGenerateCb_EmptyTask(tf::Subflow &subflow,
                        buildcc::CustomGeneratorContext &ctx) {
  (void)subflow;
  std::unordered_map<std::string, tf::Task> uom;
  for (const auto &miter : ctx.selected_schema) {
    tf::Task task;
    uom.emplace(miter.first, task);
  }
  return uom;
}

// Does not provide existing ids, plus adds random id that does not exist
static std::unordered_map<std::string, tf::Task>
BadGenerateCb_WrongId(tf::Subflow &subflow,
                      buildcc::CustomGeneratorContext &ctx) {
  (void)ctx;
  std::unordered_map<std::string, tf::Task> uom;
  auto task = subflow.placeholder();
  uom.emplace("random_id_that_does_not_exist", task);
  return uom;
}

TEST(CustomGeneratorTestGroup, FailureCases) {
  {
    buildcc::CustomGenerator cgen("failure_no_cb", "");
    buildcc::GenerateCb cb;
    cgen.AddGenerateCb(cb);
    CHECK_THROWS(std::exception, cgen.Build());
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::CustomGenerator cgen("failure_empty_task_cb", "");
    cgen.AddGenerateCb(BadGenerateCb_EmptyTask);
    cgen.AddRelInputOutput("id1", {"{gen_root_dir}/dummy_main.cpp"},
                           {"{gen_build_dir}/dummy_main.o"});
    cgen.Build();
    buildcc::m::CustomGeneratorRunner(cgen);

    CHECK_TRUE(buildcc::env::get_task_state() ==
               buildcc::env::TaskState::FAILURE);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::CustomGenerator cgen("failure_wrong_id_cb", "");
    cgen.AddGenerateCb(BadGenerateCb_WrongId);
    cgen.AddRelInputOutput("id1", {"{gen_root_dir}/dummy_main.cpp"},
                           {"{gen_build_dir}/dummy_main.o"});
    cgen.Build();
    buildcc::m::CustomGeneratorRunner(cgen);

    CHECK_TRUE(buildcc::env::get_task_state() ==
               buildcc::env::TaskState::FAILURE);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::CustomGenerator cgen("failure_cannot_save", "");
    fs::create_directory(
        cgen.GetBinaryPath()); // make a folder so that file cannot be saved

    cgen.AddGenerateCb(BasicGenerateCb);
    cgen.AddRelInputOutput("id1", {}, {});
    cgen.AddRelInputOutput("id2", {}, {});
    cgen.Build();

    mock().expectOneCall("BasicGenerateCb");
    mock().expectOneCall("id1");
    mock().expectOneCall("id2");
    buildcc::m::CustomGeneratorRunner(cgen);

    CHECK_TRUE(buildcc::env::get_task_state() ==
               buildcc::env::TaskState::FAILURE);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::CustomGenerator cgen("gen_task_not_run_no_io", "");
    cgen.AddGenerateCb(BasicGenerateCb);
    cgen.Build();

    mock().expectOneCall("BasicGenerateCb");
    buildcc::m::CustomGeneratorRunner(cgen);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

static std::unordered_map<std::string, tf::Task>
RealGenerateCb(tf::Subflow &subflow, buildcc::CustomGeneratorContext &ctx) {
  mock().actualCall("RealGenerateCb");
  std::unordered_map<std::string, tf::Task> uom;
  for (const auto &miter : ctx.selected_schema) {
    auto task = subflow.emplace([&]() {
      try {
        bool executed = buildcc::env::Command::Execute("");
        buildcc::env::assert_fatal(executed, "");
        ctx.Success(miter.first);
        mock().actualCall(fmt::format("{}:SUCCESS", miter.first).c_str());
      } catch (...) {
        ctx.Failure();
        mock().actualCall(fmt::format("{}:FAILURE", miter.first).c_str());
      }
    });
    uom.emplace(miter.first, task);
  }
  return uom;
}

TEST(CustomGeneratorTestGroup, RealGenerate) {
  constexpr const char *const kGenName = "real_generator";
  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddGenerateCb(RealGenerateCb);
    cgen.AddRelInputOutput("id1", {"{gen_root_dir}/dummy_main.cpp"},
                           {"{gen_build_dir}/dummy_main.o"});
    cgen.AddRelInputOutput("id2", {"{gen_root_dir}/dummy_main.c"},
                           {"{gen_build_dir}/dummy_main.o"});
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    mock().expectOneCall("id1:SUCCESS");
    mock().expectOneCall("id2:SUCCESS");
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_rels_map.size(), 2);

    fs::remove_all(cgen.GetBinaryPath());
  }

  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddGenerateCb(RealGenerateCb);
    cgen.AddRelInputOutput("id1", {"{gen_root_dir}/dummy_main.cpp"}, {});
    cgen.AddRelInputOutput("id2", {"{gen_root_dir}/dummy_main.c"}, {});
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, false);
    buildcc::env::m::CommandExpect_Execute(1, true);
    mock().expectOneCall("id1:FAILURE");
    mock().expectOneCall("id2:SUCCESS");
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
