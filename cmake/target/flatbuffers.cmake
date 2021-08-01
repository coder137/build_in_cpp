file(GLOB FLATBUFFERS_INCLUDE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers/include/flatbuffers/*.h")
if(${BUILDCC_PRECOMPILE_HEADERS})
    list(APPEND FLATBUFFERS_INCLUDE_HEADERS ${FBS_DIR}/pch/pch.h)
endif()

add_library(flatbuffers_header_only INTERFACE
    ${FLATBUFFERS_INCLUDE_HEADERS}
)
target_include_directories(flatbuffers_header_only INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers/include>
    $<INSTALL_INTERFACE:"include/flatbuffers">
)

if (${BUILDCC_INSTALL})
    install(TARGETS flatbuffers_header_only DESTINATION lib EXPORT flatbuffers_header_onlyConfig)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers/include/ DESTINATION "include")
    install(EXPORT flatbuffers_header_onlyConfig DESTINATION "lib/cmake/flatbuffers_header_only")
endif()
