project(target)

include(cmake/fbs_gen.cmake)

# Target
include(cmake/target.cmake)

# Tests
if (BUILDCC_TESTING)
    include(cmake/mock_target.cmake)
    add_subdirectory(test)
endif()
