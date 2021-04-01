add_library(target_interface INTERFACE)
target_sources(target_interface INTERFACE
    ${TARGET_DIR}/src/target.cpp
    ${TARGET_DIR}/src/toolchain.cpp

    ${TARGET_DIR}/src/fbs_loader.cpp
    ${TARGET_DIR}/src/fbs_storer.cpp

    ${TARGET_DIR}/src/util.cpp

    ${TARGET_DIR}/src/static_target.cpp
    ${TARGET_DIR}/src/dynamic_target.cpp   
)
target_include_directories(target_interface INTERFACE
    ${TARGET_DIR}/include
    ${TARGET_DIR}/generated
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(target_interface INTERFACE
    env
    flatbuffers
    CppUTest
    CppUTestExt
    gcov
)
target_compile_options(target_interface INTERFACE -g -fprofile-arcs -ftest-coverage -O0)
target_link_options(target_interface INTERFACE -g -fprofile-arcs -ftest-coverage -O0)
