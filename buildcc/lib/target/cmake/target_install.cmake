install(TARGETS target DESTINATION lib EXPORT targetConfig)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
install(EXPORT targetConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target")
