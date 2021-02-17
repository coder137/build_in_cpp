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
