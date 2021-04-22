add_library(clang_compile_commands
    clang_compile_commands.cpp
    clang_compile_commands.h
)
target_include_directories(clang_compile_commands PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(clang_compile_commands PRIVATE target flatbuffers)

# Target_gcc Install
if (${BUILDCC_INSTALL})
    install(TARGETS clang_compile_commands DESTINATION lib EXPORT clang_compile_commandsConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/clang_compile_commands.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT clang_compile_commandsConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/clang_compile_commands")
endif()
