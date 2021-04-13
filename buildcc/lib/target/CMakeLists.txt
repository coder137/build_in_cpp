project(target)

include(cmake/fbs_gen.cmake)

# Target
include(cmake/target.cmake)

# Mock Target

# Tests
if (BUILDCC_TESTING)
    set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    include(cmake/mock_target.cmake)
    add_subdirectory(test)
endif()
