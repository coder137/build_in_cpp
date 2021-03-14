add_library(mock INTERFACE)
target_include_directories(mock INTERFACE .)
target_link_libraries(mock INTERFACE
    CppUTest
    CppUTestExt
)
