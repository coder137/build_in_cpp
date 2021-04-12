project(target)

include(cmake/fbs_gen.cmake)

# Target
include(cmake/target.cmake)

# Mock Target
include(cmake/mock_target.cmake)

# Tests
if (BUILDCC_TESTING)
    set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()
