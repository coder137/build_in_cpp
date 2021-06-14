if (${BUILDCC_INSTALL})
    install(TARGETS tiny-process-library
        EXPORT tiny-process-library-config
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

    install(EXPORT tiny-process-library-config
        FILE tiny-process-library-config.cmake
        NAMESPACE tiny-process-library::
        DESTINATION lib/cmake/tiny-process-library
        )

    install(FILES process.hpp DESTINATION include)
endif()
