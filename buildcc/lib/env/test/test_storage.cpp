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

class MyScopedStorage : public buildcc::ScopedStorage {
public:
  // We want to unit test this
  template <typename T> void Remove(T *ptr) {
    this->ScopedStorage::Remove<T>(ptr);
  }
};

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
  MyScopedStorage storage;
  storage.Add<BigObjWithParameters>("identifier", "name", 10, obj);
  storage.Add<BigObjWithParameters>("identifier2", "name2", 12, obj);

  // Usage
  storage.ConstRef<BigObjWithParameters>("identifier").GetName();
  storage.Ref<BigObjWithParameters>("identifier2").GetName();

  CHECK_TRUE(storage.Contains("identifier"));
  CHECK_FALSE(storage.Contains("identifier_does_not_exist"));

  CHECK_TRUE(storage.Valid<BigObjWithParameters>("identifier"));
  CHECK_FALSE(storage.Valid<BigObjWithParameters>("wrong_identifier"));
  CHECK_FALSE(storage.Valid<int>("identifier"));
  // Automatic cleanup here
}

TEST(ScopedStorageTestGroup, IncorrectUsage) {
  MyScopedStorage storage;
  storage.Add<BigObjWithParameters>("identifier", "name", 10, obj);

  // We try to cast to a different type!
  CHECK_THROWS(std::exception, storage.Ref<BigObj>("identifier"));

  // We use a wrong identifier
  CHECK_THROWS(std::exception,
               storage.Ref<BigObjWithParameters>("identifier2"));
}

TEST(ScopedStorageTestGroup, NullptrDelete) {
  MyScopedStorage storage;
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

TEST(StorageTestGroup, UsageWithoutInit) {
  buildcc::Storage::Deinit();

  CHECK_THROWS(std::exception, buildcc::Storage::Add<int>("integer"));
  CHECK_THROWS(std::exception, buildcc::Storage::Ref<int>("integer"));
  CHECK_THROWS(std::exception, buildcc::Storage::ConstRef<int>("integer"));
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
