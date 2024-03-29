if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(TEST_COMPILE_FLAGS -g -Og -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline -fprofile-abs-path
    )
    set(TEST_LINK_FLAGS -g -Og -fprofile-arcs -ftest-coverage -fprofile-abs-path
    )
    set(TEST_LINK_LIBS gcov)
endif()
