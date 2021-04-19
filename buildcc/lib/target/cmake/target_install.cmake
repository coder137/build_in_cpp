install(TARGETS target DESTINATION lib EXPORT targetConfig)
install(FILES  
    ${CMAKE_CURRENT_SOURCE_DIR}/include/target.h
    DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
)
install(FILES  
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/fbs_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/path.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/util.h
    DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}/internal"
)
install(EXPORT targetConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target")
