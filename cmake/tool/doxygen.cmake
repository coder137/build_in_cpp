if (${BUILDCC_DOCUMENTATION})
    find_package(Doxygen 
                REQUIRED dot
    )
    message("Doxygen Found: ${DOXYGEN_FOUND}")
    message("Doxygen Version: ${DOXYGEN_VERSION}")

    # set(DOXYGEN_EXCLUDE_PATTERNS 
    #     *test/*
    #     *mock/*
    # )
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_GENERATE_XML YES)
    doxygen_add_docs(doxygen_documentation
        ${CMAKE_CURRENT_SOURCE_DIR}/buildcc
        COMMENT "Doxygen documentation"
    )

    find_program(sphinx_build
        NAMES "sphinx-build"
        REQUIRED
    )
    add_custom_target(sphinx_documentation
    COMMAND ${sphinx_build} -b html -Dbreathe_projects.buildcc_documentation=${CMAKE_CURRENT_BINARY_DIR}/xml . ../output
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/source
    VERBATIM USES_TERMINAL
    )
endif()
