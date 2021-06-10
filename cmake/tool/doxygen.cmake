if (${BUILDCC_DOCUMENTATION})
    find_package(Doxygen 
                REQUIRED dot
    )
    message("Doxygen Found: ${DOXYGEN_FOUND}")
    message("Doxygen Version: ${DOXYGEN_VERSION}")

    set(DOXYGEN_EXCLUDE_PATTERNS 
        *test/*
        *mock/*
    )
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_EXTRACT_ALL NO)
    set(DOXYGEN_EXTRACT_PRIV_VIRTUAL YES)
    set(DOXYGEN_MARKDOWN_SUPPORT YES)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
    doxygen_add_docs(doxygen_documentation
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/TODO.md
        ${CMAKE_CURRENT_SOURCE_DIR}/example/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/buildcc
        COMMENT "Doxygen documentation"
    )
endif()
