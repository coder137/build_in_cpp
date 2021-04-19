# Target flatc generator
include(cmake/fbs_gen.cmake)

# Target lib
include(cmake/target.cmake)

# Target mocks and tests
if (${TESTING})
    include(cmake/mock_target.cmake)
    add_subdirectory(test/path)
    add_subdirectory(test/target)
endif()

# Target install
if (${BUILDCC_INSTALL})
    include(cmake/target_install.cmake)
endif()
