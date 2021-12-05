#include "env/util.h"

#include <iostream>

#include "env/host_os.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(EnvUtilTestGroup)
{
};
// clang-format on

// save_file

TEST(EnvUtilTestGroup, Util_SaveFile_NullptrInput) {
  constexpr const char *const FILENAME = "NullptrInput.txt";
  fs::remove(FILENAME);

  const char *data = nullptr;
  bool save = buildcc::env::save_file(FILENAME, data, 1, false);
  CHECK_FALSE(save);
}

TEST(EnvUtilTestGroup, Util_SaveFile_NullptrName) {
  const char *filename = nullptr;
  bool save = buildcc::env::save_file(filename, "Hello", false);
  CHECK_FALSE(save);
}

TEST(EnvUtilTestGroup, Util_SaveFile_BadWrite) {
  constexpr const char *const FILENAME = "BadWrite.txt";
  fs::remove(FILENAME);
  bool save = buildcc::env::save_file(FILENAME, "Hello", -1, false);
  CHECK_FALSE(save);
}

TEST(EnvUtilTestGroup, Util_SaveFile_GoodWrite) {
  constexpr const char *const FILENAME = "GoodWrite.txt";
  fs::remove(FILENAME);
  bool save = buildcc::env::save_file(FILENAME, "Hello", false);
  CHECK_TRUE(save);
}

TEST(EnvUtilTestGroup, Util_SaveFile_BadWrite_Binary) {
  constexpr const char *const FILENAME = "BadWrite_Binary.txt";
  fs::remove(FILENAME);
  bool save = buildcc::env::save_file(FILENAME, "Hello", -1, true);
  CHECK_FALSE(save);
}

TEST(EnvUtilTestGroup, Util_SaveFile_GoodWrite_Binary) {
  constexpr const char *const FILENAME = "GoodWrite_Binary.txt";
  fs::remove(FILENAME);
  bool save = buildcc::env::save_file(FILENAME, "Hello", true);
  CHECK_TRUE(save);
}

TEST(EnvUtilTestGroup, Util_SaveFile_CheckDirectory) {
  // NOTE, This is a directory
  constexpr const char *const DIRNAME = "my_random_directory";
  fs::create_directory(DIRNAME);
  bool save = buildcc::env::save_file(DIRNAME, "Hello", true);
  CHECK_FALSE(save);
}

TEST(EnvUtilTestGroup, Util_SaveFile_CannotWrite) {
  constexpr const char *const FILENAME = "CannotWrite.txt";
  fs::remove(FILENAME);
  bool save = buildcc::env::save_file(FILENAME, "Hello", false);
  CHECK_TRUE(save);

  std::error_code err;
  fs::permissions(FILENAME, fs::perms::none, err);
  if (err) {
    FAIL("Cannot disable file permissions");
  }

  save = buildcc::env::save_file(FILENAME, "Hello", false);
  CHECK_FALSE(save);
}

// Load File
TEST(EnvUtilTestGroup, Util_LoadFile_CheckDirectory) {
  // NOTE, This is a directory
  constexpr const char *const DIRNAME = "my_random_directory";
  fs::create_directory(DIRNAME);
  std::string str;
  bool load = buildcc::env::load_file(DIRNAME, false, &str);
  std::cout << str << std::endl;
  CHECK_FALSE(load);
}

TEST(EnvUtilTestGroup, Util_LoadFile_NullptrName) {
  const char *filename = nullptr;
  std::string str;
  bool load = buildcc::env::load_file(filename, false, &str);
  CHECK_FALSE(load);
}

TEST(EnvUtilTestGroup, Util_LoadFile_NullptrBuf) {
  constexpr const char *const FILENAME = "NullptrBuf.txt";

  std::string *str = nullptr;
  bool load = buildcc::env::load_file(FILENAME, false, str);
  CHECK_FALSE(load);
}

TEST(EnvUtilTestGroup, Util_LoadFile_NullptrBufAndName) {
  const char *filename = nullptr;
  std::string *str = nullptr;
  bool load = buildcc::env::load_file(filename, false, str);
  CHECK_FALSE(load);
}

TEST(EnvUtilTestGroup, Util_LoadFile_ReadBinary) {
  constexpr const char *const FILENAME = "ReadBinary.txt";

  char data[] = {0x00, 0x01, 0x02, 0x03, 0x04};
  bool save = buildcc::env::save_file(FILENAME, data, sizeof(data), true);
  CHECK_TRUE(save);

  std::string str;
  bool load = buildcc::env::load_file(FILENAME, true, &str);
  MEMCMP_EQUAL(data, str.data(), sizeof(data));
  CHECK_TRUE(load);
}

TEST(EnvUtilTestGroup, Util_LoadFile_ReadTxt) {
  constexpr const char *const FILENAME = "ReadTxt.txt";

  bool save = buildcc::env::save_file(FILENAME, "ReadTxt", false);
  CHECK_TRUE(save);

  std::string str;
  bool load = buildcc::env::load_file(FILENAME, false, &str);
  STRCMP_EQUAL(str.c_str(), "ReadTxt");
  CHECK_TRUE(load);
}

TEST(EnvUtilTestGroup, Util_LoadFile_CannotOpen) {
  constexpr const char *const FILENAME = "CannotOpen.txt";
  buildcc::env::save_file(FILENAME, "Random Data", false);

  // Remove read permission
  std::error_code err;
  fs::permissions(FILENAME, fs::perms::none, err);
  if (err) {
    FAIL("Cannot disable file permissions");
  }

  std::string str;
  bool load = buildcc::env::load_file(FILENAME, true, &str);
  CHECK_FALSE(load);
}

TEST(EnvUtilTestGroup, Util_Split) {
  {
    std::vector<std::string> paths = buildcc::env::split("", ':');
    CHECK_EQUAL(paths.size(), 0);
  }

  {
    std::vector<std::string> paths = buildcc::env::split("path1", ':');
    CHECK_EQUAL(paths.size(), 1);
    STRCMP_EQUAL(paths[0].c_str(), "path1");
  }

  {
    std::vector<std::string> paths =
        buildcc::env::split("path1:path2:path3", ':');
    CHECK_EQUAL(paths.size(), 3);
    STRCMP_EQUAL(paths[0].c_str(), "path1");
    STRCMP_EQUAL(paths[1].c_str(), "path2");
    STRCMP_EQUAL(paths[2].c_str(), "path3");
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
