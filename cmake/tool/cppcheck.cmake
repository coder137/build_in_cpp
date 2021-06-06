if(${BUILDCC_CPPCHECK})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cppcheck_output)
    set(CPPCHECK_ENABLE --enable=all)
    set(CPPCHECK_PATH_SUPPRESS 
        --suppress=*:*test/* 
        --suppress=*:*mock/*
    )
    set(CPPCHECK_TAG_SUPPRESS 
        --suppress=missingInclude
        --suppress=unusedFunction
        --suppress=unmatchedSuppression
    )
    set(CPPCHECK_ADDITIONAL_OPTIONS 
        --std=c++17
        -q 
        --error-exitcode=1
        --cppcheck-build-dir=${CMAKE_CURRENT_BINARY_DIR}/cppcheck_output
    )
    set(CPPCHECK_CHECK_DIR 
        ${CMAKE_CURRENT_SOURCE_DIR}/buildcc
    )
    add_custom_target(cppcheck_static_analysis
        COMMAND cppcheck 
        ${CPPCHECK_ENABLE}
        ${CPPCHECK_PATH_SUPPRESS}
        ${CPPCHECK_TAG_SUPPRESS}
        ${CPPCHECK_ADDITIONAL_OPTIONS}
        ${CPPCHECK_CHECK_DIR}
        COMMENT "Cppcheck Static Analysis"
        VERBATIM USES_TERMINAL
    )
endif()
