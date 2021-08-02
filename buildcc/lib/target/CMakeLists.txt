# Target mocks and tests
if (${TESTING})
    set(TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    include(cmake/mock_target.cmake)
    add_subdirectory(test/path)
    add_subdirectory(test/target)
endif()

# Target flatc generator
include(cmake/fbs_gen.cmake)

# Target lib
include(cmake/target.cmake)

# Target install
if (${BUILDCC_INSTALL} AND ${BUILDCC_BUILD_AS_INTERFACE})
    include(cmake/target_install.cmake)
endif()
