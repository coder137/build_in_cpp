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
# NOTE, Added buildcc as OBJECT library to reduce link time
add_library(buildcc)
target_include_directories(buildcc PUBLIC 
    include
)
add_dependencies(buildcc fbs_to_header)

# PRIVATE linkages
target_sources(buildcc PRIVATE 
    src/target.cpp
    src/fbs_utils.cpp
    src/fbs_loader.cpp
)
target_include_directories(buildcc PRIVATE
    generated
)
target_link_libraries(buildcc PRIVATE 
    flatbuffers 
    spdlog::spdlog
)

# Mocks
add_subdirectory(mock)

# Tests
if (BUILDCC_TESTING)
    set(BUILDCC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()
