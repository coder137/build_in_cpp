#include "target/custom_generator.h"

#include "expect_command.h"
#include "expect_custom_generator.h"
#include "test_target_util.h"

#include <memory>

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
  cgen.AddIdInfo("id1", {"{current_root_dir}/dummy_main.c"},
                 {"{current_build_dir}/dummy_main.o"}, BasicGenerateCb);
  cgen.AddIdInfo("id2", {"{current_root_dir}/dummy_main.cpp"}, {},
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

    const auto &internal_map = serialization.GetLoad().internal_ids;
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
  cgen.AddIdInfo("id1", {"{current_root_dir}/dummy_main.c"}, {},
                 BasicGenerateCb);
  cgen.AddIdInfo("id2", {"{current_root_dir}/dummy_main.cpp"}, {},
                 BasicGenerateCb);
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  mock().expectOneCall("BasicGenerateCb").andReturnValue(false);
  buildcc::m::CustomGeneratorRunner(cgen);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);

  // Load
  buildcc::internal::CustomGeneratorSerialization serialization(
      cgen.GetBinaryPath());
  CHECK_TRUE(serialization.LoadFromFile());

  const auto &internal_map = serialization.GetLoad().internal_ids;
  CHECK_EQUAL(internal_map.size(), 1);
}

TEST(CustomGeneratorTestGroup, Basic_Group) {
  buildcc::CustomGenerator cgen("basic_group", "");
  cgen.AddIdInfo("id1", {"{current_root_dir}/dummy_main.c"},
                 {"{current_build_dir}/dummy_main.o"}, BasicGenerateCb);
  cgen.AddIdInfo("id2", {"{current_root_dir}/dummy_main.cpp"}, {},
                 BasicGenerateCb);
  cgen.AddGroupInfo("grouped_id1_and_id2", {"id1", "id2"});
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  buildcc::m::CustomGeneratorRunner(cgen);

  // Serialization check
  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());

    const auto &internal_map = serialization.GetLoad().internal_ids;
    CHECK_EQUAL(internal_map.size(), 2);
    const auto &id1_info = internal_map.at("id1");
    CHECK_EQUAL(id1_info.internal_inputs.size(), 1);
    CHECK_EQUAL(id1_info.outputs.size(), 1);

    const auto &id2_info = internal_map.at("id2");
    CHECK_EQUAL(id2_info.internal_inputs.size(), 1);
    CHECK_EQUAL(id2_info.outputs.size(), 0);
  }
}

bool FailureCb(const buildcc::CustomGeneratorContext &ctx) {
  (void)ctx;
  return false;
}
bool SuccessCb(const buildcc::CustomGeneratorContext &ctx) {
  (void)ctx;
  return true;
}

TEST(CustomGeneratorTestGroup, DefaultArgumentUsage) {
  buildcc::CustomGenerator cgen("default_argument_usage", "");
  cgen.AddPatterns({
      {"dummy_main_c", "{current_root_dir}/dummy_main.c"},
      {"dummy_main_o", "{current_build_dir}/dummy_main.o"},
      {"dummy_main_cpp", "{current_root_dir}/dummy_main.cpp"},
  });
  cgen.AddIdInfo("id1", {"{dummy_main_c}"}, {"{dummy_main_o}"},
                 BasicGenerateCb);
  cgen.AddIdInfo("id2", {"{dummy_main_cpp}"}, {}, BasicGenerateCb);
  cgen.Build();

  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  mock().expectOneCall("BasicGenerateCb").andReturnValue(true);
  buildcc::m::CustomGeneratorRunner(cgen);

  // Serialization check
  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());

    const auto &internal_map = serialization.GetLoad().internal_ids;
    CHECK_EQUAL(internal_map.size(), 2);
    const auto &id1_info = internal_map.at("id1");
    CHECK_EQUAL(id1_info.internal_inputs.size(), 1);
    CHECK_EQUAL(id1_info.outputs.size(), 1);

    const auto &id2_info = internal_map.at("id2");
    CHECK_EQUAL(id2_info.internal_inputs.size(), 1);
    CHECK_EQUAL(id2_info.outputs.size(), 0);
  }
}

TEST(CustomGeneratorTestGroup, FailureCases) {
  {
    buildcc::CustomGenerator cgen("failure_no_cb", "");
    buildcc::GenerateCb cb;
    CHECK_THROWS(std::exception, cgen.AddIdInfo("id1", {}, {}, cb));
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  {
    buildcc::CustomGenerator cgen("failure_cannot_save", "");
    fs::create_directory(
        cgen.GetBinaryPath()); // make a folder so that file cannot be saved

    cgen.AddIdInfo("id1", {}, {}, BasicGenerateCb);
    cgen.AddIdInfo("id2", {}, {}, BasicGenerateCb);
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
    cgen.AddIdInfo("id1", {}, {}, BasicGenerateCb);
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

TEST(CustomGeneratorTestGroup, RealGenerate_Basic) {
  constexpr const char *const kGenName = "real_generator_basic";
  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddIdInfo("id1", {"{current_root_dir}/dummy_main.cpp"},
                   {"{current_build_dir}/dummy_main.o"}, RealGenerateCb);
    cgen.AddIdInfo("id2", {"{current_root_dir}/dummy_main.c"},
                   {"{current_build_dir}/dummy_main.o"}, RealGenerateCb);
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_ids.size(), 2);

    fs::remove_all(cgen.GetBinaryPath());
  }

  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddIdInfo("id1", {"{current_root_dir}/dummy_main.cpp"}, {},
                   RealGenerateCb);
    cgen.AddIdInfo("id2", {"{current_root_dir}/dummy_main.c"}, {},
                   RealGenerateCb);
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, false);

    // Since there is an error above, second command does not execute (note,
    // this is the behaviour in a single thread that is why we can
    // check sequentially)
    buildcc::m::CustomGeneratorRunner(cgen);

    CHECK_TRUE(buildcc::env::get_task_state() ==
               buildcc::env::TaskState::FAILURE);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_ids.size(), 0);

    fs::remove_all(cgen.GetBinaryPath());
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(CustomGeneratorTestGroup, RealGenerate_Update_Success) {
  constexpr const char *const kGenName = "real_generator_update_success";

  {
    buildcc::CustomGenerator cgen(kGenName, "");
    buildcc::env::save_file(
        (cgen.GetBuildDir() / "dummy_main.c").string().c_str(), "", false);
    buildcc::env::save_file(
        (cgen.GetBuildDir() / "dummy_main.cpp").string().c_str(), "", false);

    cgen.AddIdInfo("id1", {"{current_build_dir}/dummy_main.c"},
                   {"{current_build_dir}/dummy_main.o"}, RealGenerateCb);
    cgen.AddIdInfo("id2", {"{current_build_dir}/dummy_main.cpp"},
                   {"{current_build_dir}/other_dummy_main.o"}, RealGenerateCb);
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_ids.size(), 2);
    auto imap = serialization.GetLoad().internal_ids;
    CHECK_EQUAL(imap.at("id1").internal_inputs.size(), 1);
    CHECK_EQUAL(imap.at("id2").internal_inputs.size(), 1);

    CHECK_EQUAL(imap.at("id1").outputs.size(), 1);
    CHECK_EQUAL(imap.at("id2").outputs.size(), 1);
  }

  buildcc::m::blocking_sleep(1);

  // Updated Input file Success
  UT_PRINT("Updated Input file: Success\r\n");
  {
    buildcc::CustomGenerator cgen(kGenName, "");
    buildcc::env::save_file(
        (cgen.GetBuildDir() / "dummy_main.cpp").string().c_str(), "", false);

    std::uint64_t last_write_timestamp = static_cast<uint64_t>(
        fs::last_write_time(cgen.GetBuildDir() / "dummy_main.cpp")
            .time_since_epoch()
            .count());

    cgen.AddIdInfo("id1", {"{current_build_dir}/dummy_main.c"},
                   {"{current_build_dir}/dummy_main.o"}, RealGenerateCb);
    cgen.AddIdInfo("id2", {"{current_build_dir}/dummy_main.cpp"},
                   {"{current_build_dir}/other_dummy_main.o"}, RealGenerateCb);
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_ids.size(), 2);
    auto imap = serialization.GetLoad().internal_ids;
    CHECK_EQUAL(imap.at("id1").internal_inputs.size(), 1);
    CHECK_EQUAL(imap.at("id1").outputs.size(), 1);

    CHECK_EQUAL(imap.at("id2").internal_inputs.size(), 1);
    CHECK_EQUAL(imap.at("id2").outputs.size(), 1);

    CHECK_EQUAL(last_write_timestamp,
                imap.at("id2").internal_inputs.begin()->last_write_timestamp);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }

  // Updated Output file Success
  UT_PRINT("Updated Output file: Success\r\n");
  {
    buildcc::CustomGenerator cgen(kGenName, "");

    cgen.AddIdInfo("id1", {"{current_build_dir}/dummy_main.c"},
                   {"{current_build_dir}/dummy_main.o"}, RealGenerateCb);
    cgen.AddIdInfo("id2", {"{current_build_dir}/dummy_main.cpp"},
                   {"{current_build_dir}/rename_dummy_main.o"}, RealGenerateCb);
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_ids.size(), 2);
    auto imap = serialization.GetLoad().internal_ids;
    CHECK_EQUAL(imap.at("id1").internal_inputs.size(), 1);
    CHECK_EQUAL(imap.at("id1").outputs.size(), 1);

    CHECK_EQUAL(imap.at("id2").internal_inputs.size(), 1);
    CHECK_EQUAL(imap.at("id2").outputs.size(), 1);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }
}

class MyCustomBlobHandler : public buildcc::CustomBlobHandler {
public:
  MyCustomBlobHandler(int32_t my_recheck_value)
      : recheck_value(my_recheck_value) {}

private:
  int32_t recheck_value = 0;

private:
  bool Verify(const std::vector<uint8_t> &serialized_data) const override {
    json j = json::from_msgpack(serialized_data);
    return !j.is_discarded();
  }

  bool IsEqual(const std::vector<uint8_t> &previous,
               const std::vector<uint8_t> &current) const override {
    return Deserialize(previous) == Deserialize(current);
  }

  std::vector<uint8_t> Serialize() const override {
    json j = recheck_value;
    return json::to_msgpack(j);
  }

  // serialized_data has already been verified
  int32_t Deserialize(const std::vector<uint8_t> &serialized_data) const {
    json j = json::from_msgpack(serialized_data);
    int32_t deserialized;
    j.get_to(deserialized);
    return deserialized;
  }
};

TEST(CustomGeneratorTestGroup, RealGenerate_BasicBlobRecheck) {
  constexpr const char *const kGenName = "real_generator_basic_blob_recheck";
  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddIdInfo("id1", {"{current_root_dir}/dummy_main.cpp"},
                   {"{current_build_dir}/dummy_main.o"}, RealGenerateCb,
                   std::make_shared<MyCustomBlobHandler>(12));
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_ids.size(), 1);
  }

  // Rebuild
  {
    buildcc::CustomGenerator cgen(kGenName, "");
    cgen.AddIdInfo("id1", {"{current_root_dir}/dummy_main.cpp"},
                   {"{current_build_dir}/dummy_main.o"}, RealGenerateCb,
                   std::make_shared<MyCustomBlobHandler>(200));
    cgen.Build();

    mock().expectOneCall("RealGenerateCb");
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(cgen);

    buildcc::internal::CustomGeneratorSerialization serialization(
        cgen.GetBinaryPath());
    CHECK_TRUE(serialization.LoadFromFile());
    CHECK_EQUAL(serialization.GetLoad().internal_ids.size(), 1);
  }

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::Project::Init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
