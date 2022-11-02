#include "schema/custom_generator_serialization.h"

#include "schema/private/schema_util.h"

#include "flatbuffers/flatbuffers.h"

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

TEST(CustomGeneratorSerializationTestGroup, FormatEmptyCheck) {
  buildcc::internal::CustomGeneratorSerialization serialization(
      "dump/FormatEmptyCheck.json");

  bool stored = serialization.StoreToFile();
  CHECK_TRUE(stored);
}

TEST(CustomGeneratorSerializationTestGroup, EmptyFile_Failure) {
  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/EmptyFile.json");
    CHECK_FALSE(serialization.LoadFromFile());
  }

  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/EmptyFile.json");
    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            "", false);
    CHECK_FALSE(serialization.LoadFromFile());
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
