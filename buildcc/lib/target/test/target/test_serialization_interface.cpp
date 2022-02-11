#include "target/interface/serialization_interface.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

class TestSerializationInterface
    : public buildcc::internal::SerializationInterface {
public:
  TestSerializationInterface(const fs::path &serialized_file)
      : SerializationInterface(serialized_file) {}

  void VerifyExpectation(int calls, bool return_value) {
    mock()
        .expectNCalls(calls, "verify")
        .onObject(this)
        .andReturnValue(return_value);
  }

  void LoadExpectation(int calls, bool return_value) {
    mock()
        .expectNCalls(calls, "load")
        .onObject(this)
        .andReturnValue(return_value);
  }

  void StoreExpectation(int calls, bool return_value) {
    mock()
        .expectNCalls(calls, "store")
        .onObject(this)
        .andReturnValue(return_value);
  }

private:
  bool Verify(const std::string &serialized_data) override {
    (void)serialized_data;
    return mock().actualCall("verify").onObject(this).returnBoolValue();
  }

  bool Load(const std::string &serialized_data) override {
    (void)serialized_data;
    return mock().actualCall("load").onObject(this).returnBoolValue();
  }

  bool Store(const fs::path &absolute_serialized_file) override {
    (void)absolute_serialized_file;
    return mock().actualCall("store").onObject(this).returnBoolValue();
  }
};

// clang-format off
TEST_GROUP(TestSerializationInterfaceGroup)
{
    void teardown() {
        mock().clear();
    }
};
// clang-format on

TEST(TestSerializationInterfaceGroup, Verify_False) {
  TestSerializationInterface test_serialization_interface(
      fs::current_path() / "data" / "dummy_main.c");

  test_serialization_interface.VerifyExpectation(1, false);
  bool loaded = test_serialization_interface.LoadFromFile();
  CHECK_FALSE(loaded);
}

TEST(TestSerializationInterfaceGroup, Load_False) {
  TestSerializationInterface test_serialization_interface(
      fs::current_path() / "data" / "dummy_main.c");

  test_serialization_interface.VerifyExpectation(1, true);
  test_serialization_interface.LoadExpectation(1, false);
  bool loaded = test_serialization_interface.LoadFromFile();
  CHECK_FALSE(loaded);
}

TEST(TestSerializationInterfaceGroup, Load_True) {
  TestSerializationInterface test_serialization_interface(
      fs::current_path() / "data" / "dummy_main.c");

  test_serialization_interface.VerifyExpectation(1, true);
  test_serialization_interface.LoadExpectation(1, true);
  bool loaded = test_serialization_interface.LoadFromFile();
  CHECK_TRUE(loaded);
}

TEST(TestSerializationInterfaceGroup, Store_False) {
  TestSerializationInterface test_serialization_interface(
      fs::current_path() / "data" / "dummy_main.c");
  test_serialization_interface.StoreExpectation(1, false);
  bool stored = test_serialization_interface.StoreToFile();
  CHECK_FALSE(stored);
}

TEST(TestSerializationInterfaceGroup, Store_True) {
  TestSerializationInterface test_serialization_interface(
      fs::current_path() / "data" / "dummy_main.c");
  test_serialization_interface.StoreExpectation(1, true);
  bool stored = test_serialization_interface.StoreToFile();
  CHECK_TRUE(stored);

  std::string serialized_file =
      test_serialization_interface.GetSerializedFile().string();
  std::string compare = (fs::current_path() / "data" / "dummy_main.c").string();
  STRCMP_EQUAL(serialized_file.c_str(), compare.c_str());
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
