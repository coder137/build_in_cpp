#include "schema/custom_generator_serialization.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(CustomGeneratorSerializationTestGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

TEST(CustomGeneratorSerializationTestGroup, JsonParse_Failure) {
  {
    // JSON Parse fails
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/CustomGeneratorJsonParseFailure.json");

    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            std::string(""), false);
    bool loaded = serialization.LoadFromFile();
    CHECK_FALSE(loaded);
  }

  {
    // Custom Generator Schema conversion fails
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/CustomGeneratorJsonParseFailure.json");

    auto data = R"({"name": ""})";
    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            data, false);
    bool loaded = serialization.LoadFromFile();
    CHECK_FALSE(loaded);
  }
}

TEST(CustomGeneratorSerializationTestGroup, FormatEmptyCheck) {
  buildcc::internal::CustomGeneratorSerialization serialization(
      "dump/CustomGeneratorFormatEmptyCheck.json");

  bool stored = serialization.StoreToFile();
  CHECK_TRUE(stored);
}

TEST(CustomGeneratorSerializationTestGroup, EmptyFile_Failure) {
  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/CustomGeneratorEmptyFile.json");
    CHECK_FALSE(serialization.LoadFromFile());
  }

  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/CustomGeneratorEmptyFile.json");
    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            "", false);
    CHECK_FALSE(serialization.LoadFromFile());
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
