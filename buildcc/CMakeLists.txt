# Generate files
set(FBS_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/fbs/target.fbs
)
set(FBS_GEN_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/target_generated.h
)
set(FBS_GEN_OPTIONS 
    --gen-object-api
)
add_custom_command(OUTPUT ${FBS_GEN_FILES}
    COMMAND flatc -o ${CMAKE_CURRENT_SOURCE_DIR}/generated ${FBS_GEN_OPTIONS} --cpp ${FBS_FILES}
    DEPENDS flatc ${FBS_FILES}
)

add_custom_target(fbs_to_header
    DEPENDS ${FBS_GEN_FILES}
)

# Library
add_library(buildcc)
target_sources(buildcc PRIVATE 
    src/target.cpp
    src/fbs_utils.cpp
)
target_include_directories(buildcc PUBLIC include)
add_dependencies(buildcc fbs_to_header)

# PRIVATE linkages
target_include_directories(buildcc PRIVATE
    ${FlatBuffers_SOURCE_DIR}/include
    generated
    fbs
)
target_link_libraries(buildcc PRIVATE flatbuffers spdlog::spdlog)
