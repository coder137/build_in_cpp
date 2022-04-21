#include "env/storage.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(ScopedStorageTestGroup)
{
};

TEST_GROUP(StorageTestGroup)
{
  void setup() {
    buildcc::Storage::Init();
  }
  void teardown() {
    buildcc::Storage::Deinit();
  }
};
// clang-format on

class BigObj {};

class BigObjWithParameters {
public:
  BigObjWithParameters(const std::string &name, int id, const BigObj &obj)
      : name_(name) {
    (void)id;
    (void)obj;
  }

  const std::string &GetName() const { return name_; }

private:
  std::string name_;
};

static BigObj obj;

TEST(ScopedStorageTestGroup, BasicUsage) {
  buildcc::ScopedStorage storage;
  storage.Add<BigObjWithParameters>("identifier", "name", 10, obj);
  storage.Add<BigObjWithParameters>("identifier2", "name2", 12, obj);

  // Usage
  storage.ConstRef<BigObjWithParameters>("identifier").GetName();
  storage.Ref<BigObjWithParameters>("identifier2").GetName();

  // Automatic cleanup here
}

TEST(ScopedStorageTestGroup, IncorrectUsage) {
  buildcc::ScopedStorage storage;
  storage.Add<BigObjWithParameters>("identifier", "name", 10, obj);

  // We try to cast to a different type!
  CHECK_THROWS(std::exception, storage.Ref<BigObj>("identifier"));

  // We use a wrong identifier
  CHECK_THROWS(std::exception,
               storage.Ref<BigObjWithParameters>("identifier2"));
}

std::string &toReference(std::string *pointer) { return *pointer; }

TEST(ScopedStorageTestGroup, NullptrDelete) {
  buildcc::ScopedStorage storage;
  storage.Remove<std::string>(nullptr);
}

TEST(StorageTestGroup, BasicUsage) {
  buildcc::Storage::Add<BigObjWithParameters>("identifier", "name", 10, obj);
  buildcc::Storage::Add<BigObjWithParameters>("identifier2", "name2", 12, obj);

  // Usage
  const auto &bigobj =
      buildcc::Storage::ConstRef<BigObjWithParameters>("identifier").GetName();
  const auto &bigobj2 =
      buildcc::Storage::Ref<BigObjWithParameters>("identifier2").GetName();

  STRCMP_EQUAL(bigobj.c_str(), "name");
  STRCMP_EQUAL(bigobj2.c_str(), "name2");
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
