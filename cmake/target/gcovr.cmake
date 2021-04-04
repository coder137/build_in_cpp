
find_program(gcovr_program
NAMES "gcovr"
)

if(${gcovr_program} STREQUAL "gcovr_program-NOTFOUND")
message("GCOVR not found, removing 'gcovr_coverage' target")
else()
message("GCOVR at ${gcovr_program}")

set(GCOVR_REMOVE_OPTIONS 
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
    ${CMAKE_BINARY_DIR}
)
set(GCOVR_DIR ${CMAKE_BINARY_DIR}/gcovr_coverage)
add_custom_target(gcovr_coverage

    COMMAND ${gcovr_program} -r ${CMAKE_CURRENT_SOURCE_DIR} --html-details ${GCOVR_DIR}/gcovr.html --object-directory ${GCOVR_DIR} ${GCOVR_VERBOSE} ${GCOVR_UNNECESSARY_ARCS} ${GCOVR_REMOVE_OPTIONS} ${GCOVR_SEARCH_PATHS}

    DEPENDS buildcc test

    VERBATIM USES_TERMINAL
)
endif()
