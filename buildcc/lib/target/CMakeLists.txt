project(target)

include(cmake/fbs_gen.cmake)

# Target
include(cmake/target.cmake)

# Tests
if (${TESTING})
    include(cmake/mock_target.cmake)
    add_subdirectory(test/path)
    add_subdirectory(test/target)
endif()
