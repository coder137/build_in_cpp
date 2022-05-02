#include "schema/custom_generator_serialization.h"

#include "schema/private/schema_util.h"

#include "flatbuffers/flatbuffers.h"

#include "custom_generator_generated.h"

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
      "dump/FormatEmptyCheck.bin");

  {
    flatbuffers::FlatBufferBuilder builder;
    // Entire std::vector<RelInputOutput> is nullptr
    auto fbs_generator =
        schema::internal::CreateCustomGeneratorDirect(builder, "", nullptr);
    schema::internal::FinishCustomGeneratorBuffer(builder, fbs_generator);

    CHECK_TRUE(buildcc::env::save_file(
        serialization.GetSerializedFile().string().c_str(),
        (const char *)builder.GetBufferPointer(), builder.GetSize(), true));

    CHECK_FALSE(serialization.LoadFromFile());

    fs::remove_all(serialization.GetSerializedFile());
  }

  {
    flatbuffers::FlatBufferBuilder builder;
    // RelInputOutput in nullptr
    auto rel_io = schema::internal::CreateRelInputOutputDirect(builder, "");
    std::vector v{rel_io};
    auto fbs_generator =
        schema::internal::CreateCustomGeneratorDirect(builder, "", &v);
    schema::internal::FinishCustomGeneratorBuffer(builder, fbs_generator);

    CHECK_TRUE(buildcc::env::save_file(
        serialization.GetSerializedFile().string().c_str(),
        (const char *)builder.GetBufferPointer(), builder.GetSize(), true));

    CHECK_TRUE(serialization.LoadFromFile());

    fs::remove_all(serialization.GetSerializedFile());
  }
}

TEST(CustomGeneratorSerializationTestGroup, EmptyFile_Failure) {
  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/empty_file.bin");
    CHECK_FALSE(serialization.LoadFromFile());
  }

  {
    buildcc::internal::CustomGeneratorSerialization serialization(
        "dump/empty_file.bin");
    char data[] = {0};
    buildcc::env::save_file(serialization.GetSerializedFile().string().c_str(),
                            (const char *)data, 1, true);
    CHECK_FALSE(serialization.LoadFromFile());
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
