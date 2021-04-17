if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(BUILD_COMPILE_FLAGS -Wall -Wextra)
    set(BUILD_LINK_FLAGS )
# TODO, Add other compiler flags here
endif()
