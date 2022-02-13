# Generate files
set(SCHEMA_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated CACHE PATH "Generate path of flatbuffer schema")

set(FBS_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/path.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/generator.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/target.fbs
)
set(FBS_GEN_FILES
    ${SCHEMA_BUILD_DIR}/path_generated.h
    ${SCHEMA_BUILD_DIR}/generator_generated.h
    ${SCHEMA_BUILD_DIR}/target_generated.h
)
set(FBS_GEN_OPTIONS 
    -I ${CMAKE_CURRENT_SOURCE_DIR}
    --gen-object-api
)

add_custom_command(OUTPUT ${FBS_GEN_FILES}
    COMMAND flatc -o ${SCHEMA_BUILD_DIR} ${FBS_GEN_OPTIONS} --cpp ${FBS_FILES}
    DEPENDS flatc ${FBS_FILES}
)

add_custom_target(fbs_to_header
    DEPENDS ${FBS_GEN_FILES}
)
