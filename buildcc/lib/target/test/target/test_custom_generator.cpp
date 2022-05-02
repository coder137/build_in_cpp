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
      mock().clear();
    }
};
// clang-format on

fs::path BUILD_DIR = fs::current_path() / "intermediate" / "custom_generator";

static std::unordered_map<std::string, tf::Task>
BasicGenerateCb(tf::Subflow &subflow, buildcc::CustomGeneratorContext &ctx) {
  mock().actualCall("BasicGenerateCb");
  std::unordered_map<std::string, tf::Task> uom;
  for (const auto &miter : ctx.selected_user_schema) {
    mock().actualCall(miter.first.c_str());
    auto task = subflow.placeholder();
    uom.emplace(miter.first, task);
  }
  return uom;
}

TEST(CustomGeneratorTestGroup, Basic) {
  buildcc::CustomGenerator cgen("custom_generator", "");
  cgen.AddGenerateCb(BasicGenerateCb);
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb");
  buildcc::m::CustomGeneratorRunner(cgen);
}

TEST(CustomGeneratorTestGroup, Basic2) {
  buildcc::CustomGenerator cgen("custom_generator2", "");
  cgen.AddGenerateCb(BasicGenerateCb);
  cgen.AddRelInputOutput("id1", {}, {});
  cgen.AddRelInputOutput("id2", {}, {});
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb");
  mock().expectOneCall("id1");
  mock().expectOneCall("id2");
  buildcc::m::CustomGeneratorRunner(cgen);
}

static std::unordered_map<std::string, tf::Task>
RealGenerateCb(tf::Subflow &subflow, buildcc::CustomGeneratorContext &ctx) {
  mock().actualCall("RealGenerateCb");
  std::unordered_map<std::string, tf::Task> uom;
  for (const auto &miter : ctx.selected_user_schema) {
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
    cgen.AddRelInputOutput("id2", {}, {});
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
    cgen.AddRelInputOutput("id1", {"{gen_root_dir}/dummy_main.cpp"},
                           {"{gen_build_dir}/dummy_main.o"});
    cgen.AddRelInputOutput("id2", {}, {});
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, false);
    buildcc::env::m::CommandExpect_Execute(1, true);
    mock().expectOneCall("id1:FAILURE");
    mock().expectOneCall("id2:SUCCESS");
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_rels_map.size(), 1);

    fs::remove_all(cgen.GetBinaryPath());
  }
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::Project::Init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
