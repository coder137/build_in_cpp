
find_program(gcovr_program
NAMES "gcovr"
)

if(${gcovr_program} STREQUAL "gcovr_program-NOTFOUND")
message("GCOVR not found, removing 'gcovr_coverage' target")
else()
message("GCOVR at ${gcovr_program}")

set(GCOVR_REMOVE_OPTIONS 
    --exclude "(.+/)?third_party(.+/)?"
    --exclude "(.+/)?spdlog(.+/)?"
    --exclude "(.+/)?fmt(.+/)?"
    --exclude "(.+/)?taskflow(.+/)?"
    --exclude "(.+/)?CLI11(.+/)?"
    --exclude "(.+/)?CppUTest(.+/)?"
    --exclude "(.+/)?CppUTestExt(.+/)?"

    --exclude "(.+/)?mock(.+/)?"
    --exclude "(.+/)?generated(.+/)?"
    --exclude "(.+/)?test(.+/)?"
)

# TODO, Update
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gcovr_coverage)
set(GCOVR_VERBOSE -v -s)
set(GCOVR_UNNECESSARY_ARCS 
    --exclude-unreachable-branches
    --exclude-throw-branches
)

set(GCOVR_SEARCH_PATHS
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(gcovr_coverage
    COMMAND cmake --build ${CMAKE_BINARY_DIR}
    COMMAND cmake --build ${CMAKE_BINARY_DIR} --target test

    COMMAND ${gcovr_program} -r ${CMAKE_CURRENT_SOURCE_DIR} --html-details ${CMAKE_BINARY_DIR}/gcovr_coverage/gcovr.html ${GCOVR_VERBOSE} ${GCOVR_UNNECESSARY_ARCS} ${GCOVR_REMOVE_OPTIONS} -j 12 ${GCOVR_SEARCH_PATHS}

    VERBATIM USES_TERMINAL
)
endif()
