macro(m_cppcheck)
    if (${BUILDCC_CPPCHECK})
        message("Setting CppCheck: ON -> ${ARGV0}")
        set(SUPPRESS_LIBS 
            --suppress=*:*flatbuffers/* 
            --suppress=*:*fmt/* 
            --suppress=*:*CLI11/* 
            --suppress=*:*cpputest/* 
            --suppress=*:*spdlog/* 
            --suppress=*:*taskflow/*
        )
        set(SUPPRESS_GLOBAL --suppress=unmatchedSuppression 
            --suppress=missingIncludeSystem 
            --suppress=unusedFunction 
        )
        set(CMAKE_CXX_CPPCHECK cppcheck --enable=all 
            --cppcheck-build-dir=${CMAKE_CURRENT_BINARY_DIR}/cppcheck_intermediate 
            --std=c++17
            ${SUPPRESS_LIBS} 
            ${SUPPRESS_GLOBAL} 
            --error-exitcode=1 
            --force 
            -q
            -D__GNUC__=10
        )
    else()
        message("Setting CppCheck: OFF -> ${ARGV0}")
    endif()
endmacro()
