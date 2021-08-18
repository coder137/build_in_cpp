# Target mocks and tests
if (${TESTING})
    set(TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    include(cmake/mock_target.cmake)
    add_subdirectory(test/path)
    add_subdirectory(test/target)
endif()

# Target lib
include(cmake/target.cmake)

# Target install
include(cmake/target_install.cmake)
