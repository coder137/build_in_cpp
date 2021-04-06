
find_program(lcov_program
    NAMES "lcov"
)

if(${lcov_program} STREQUAL "lcov_program-NOTFOUND")
    message("LCOV not found, removing 'lcov_coverage' target")
else()
    message("LCOV at ${lcov_program}")
    set(LCOV_INITIAL_FILE ${CMAKE_BINARY_DIR}/coverage.info)
    set(LCOV_TRUNCATED_FILE ${CMAKE_BINARY_DIR}/coverage_truncated.info)
    set(LCOV_RC_OPTIONS 
        --rc lcov_branch_coverage=1
        --rc genhtml_branch_coverage=1
    )
    set(LCOV_REMOVE_OPTIONS 
        "/usr*"
        "*/generated*"
        "*/CppUTestExt*"
        "*/spdlog*"
        "*/flatbuffers*"
        "*/CppUTest*"
        "*/test*"
    )
    add_custom_target(lcov_coverage
        COMMAND ${lcov_program} --zerocounters --directory ${CMAKE_SOURCE_DIR} ${LCOV_RC_OPTIONS}

        COMMAND cmake --build ${CMAKE_BINARY_DIR}
        COMMAND cmake --build ${CMAKE_BINARY_DIR} --target test

        COMMAND ${lcov_program} --capture --directory ${CMAKE_SOURCE_DIR} --output-file ${LCOV_INITIAL_FILE} ${LCOV_RC_OPTIONS}

        COMMAND ${lcov_program} --remove ${LCOV_INITIAL_FILE} ${LCOV_REMOVE_OPTIONS} --output-file ${LCOV_TRUNCATED_FILE} ${LCOV_RC_OPTIONS}

        COMMAND ${lcov_program} --list ${LCOV_TRUNCATED_FILE} ${LCOV_RC_OPTIONS}

        # TODO, Later i.e not in the same target
        COMMAND genhtml ${LCOV_TRUNCATED_FILE} --output-directory ${CMAKE_BINARY_DIR}/lcov_coverage --branch-coverage --function-coverage

        VERBATIM USES_TERMINAL
    )
endif()
