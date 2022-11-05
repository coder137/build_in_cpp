#include "schema/target_serialization.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetSerializationTestGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

TEST(TargetSerializationTestGroup, TargetTypeTest) {
  {
    // Target Type executable
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetTypeTest.json");

    buildcc::internal::TargetSchema schema;
    schema.type = buildcc::TargetType::Executable;
    serialization.UpdateStore(schema);
    bool store = serialization.StoreToFile();
    CHECK_TRUE(store);

    bool load = serialization.LoadFromFile();
    CHECK_TRUE(load);
    CHECK_TRUE(serialization.GetLoad().type == buildcc::TargetType::Executable);
  }

  {
    // Target Type static library
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetTypeTest.json");

    buildcc::internal::TargetSchema schema;
    schema.type = buildcc::TargetType::StaticLibrary;
    serialization.UpdateStore(schema);
    bool store = serialization.StoreToFile();
    CHECK_TRUE(store);

    bool load = serialization.LoadFromFile();
    CHECK_TRUE(load);
    CHECK_TRUE(serialization.GetLoad().type ==
               buildcc::TargetType::StaticLibrary);
  }

  {
    // Target Type dynamic library
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetTypeTest.json");

    buildcc::internal::TargetSchema schema;
    schema.type = buildcc::TargetType::DynamicLibrary;
    serialization.UpdateStore(schema);
    bool store = serialization.StoreToFile();
    CHECK_TRUE(store);

    bool load = serialization.LoadFromFile();
    CHECK_TRUE(load);
    CHECK_TRUE(serialization.GetLoad().type ==
               buildcc::TargetType::DynamicLibrary);
  }

  {
    // Target Type undefined
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetTypeTest.json");

    buildcc::internal::TargetSchema schema;
    schema.type = buildcc::TargetType::Undefined;
    serialization.UpdateStore(schema);
    bool store = serialization.StoreToFile();
    CHECK_TRUE(store);

    bool load = serialization.LoadFromFile();
    CHECK_TRUE(load);
    CHECK_TRUE(serialization.GetLoad().type == buildcc::TargetType::Undefined);
  }

  {
    // Target Type random value
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetTypeTest.json");

    buildcc::internal::TargetSchema schema;
    schema.type = (buildcc::TargetType)65535;
    serialization.UpdateStore(schema);
    bool store = serialization.StoreToFile();
    CHECK_TRUE(store);

    bool load = serialization.LoadFromFile();
    CHECK_TRUE(load);
    CHECK_TRUE(serialization.GetLoad().type == buildcc::TargetType::Undefined);
  }
}

TEST(TargetSerializationTestGroup, JsonParse_Failure) {
  {
    // JSON Parse fails
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetJsonParseFailure.json");

    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            std::string(""), false);
    bool loaded = serialization.LoadFromFile();
    CHECK_FALSE(loaded);
  }

  {
    // Custom Generator Schema conversion fails
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetJsonParseFailure.json");

    auto data = R"({"name": ""})";
    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            data, false);
    bool loaded = serialization.LoadFromFile();
    CHECK_FALSE(loaded);
  }
}

TEST(TargetSerializationTestGroup, FormatEmptyCheck) {
  buildcc::internal::TargetSerialization serialization(
      "dump/TargetFormatEmptyCheck.json");

  bool stored = serialization.StoreToFile();
  CHECK_TRUE(stored);

  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);
}

TEST(TargetSerializationTestGroup, EmptyFile_Failure) {
  {
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetEmptyFile.json");
    CHECK_FALSE(serialization.LoadFromFile());
  }

  {
    buildcc::internal::TargetSerialization serialization(
        "dump/TargetEmptyFile.json");
    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            "", false);
    CHECK_FALSE(serialization.LoadFromFile());
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
