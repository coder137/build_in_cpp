add_subdirectory(CLI11)
file(GLOB CLI_INCLUDE_HEADERS "${CLI11_SOURCE_DIR}/include/CLI/*.hpp")

if (${BUILDCC_INSTALL})
    install(TARGETS CLI11 DESTINATION lib EXPORT CLI11Config)
    install(FILES  
        ${CLI_INCLUDE_HEADERS}
        DESTINATION "include/CLI11")
    install(EXPORT CLI11Config DESTINATION "lib/cmake/CLI11")
endif()
