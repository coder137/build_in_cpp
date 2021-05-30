macro(m_cppcheck)
    if (${BUILDCC_CPPCHECK})
        message("Setting CppCheck: ON -> ${ARGV0}")
        set(CMAKE_CXX_CPPCHECK cppcheck --enable=all --cppcheck-build-dir=${CMAKE_CURRENT_BINARY_DIR}/cppcheck_intermediate --suppress=*:*flatbuffers/* --suppress=*:*fmt/* --suppress=*:*CLI11/* --suppress=*:*cpputest/* --suppress=*:*spdlog/* --suppress=*:*taskflow/* --suppress=unmatchedSuppression --suppress=missingIncludeSystem --suppress=unusedFunction --suppress=noValidConfiguration --error-exitcode=1 --force -q
        )
    else()
        message("Setting CppCheck: OFF -> ${ARGV0}")
    endif()
endmacro()
