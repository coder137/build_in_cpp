project(target)

# Generate files
set(FBS_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/fbs/target.fbs
)
set(FBS_GEN_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/generated/target_generated.h
)
set(FBS_GEN_OPTIONS 
    --gen-object-api
)
add_custom_command(OUTPUT ${FBS_GEN_FILES}
    COMMAND flatc -o ${CMAKE_CURRENT_BINARY_DIR}/generated ${FBS_GEN_OPTIONS} --cpp ${FBS_FILES}
    DEPENDS flatc ${FBS_FILES}
)

add_custom_target(fbs_to_header
    DEPENDS ${FBS_GEN_FILES}
)

# Target
add_library(target)
target_include_directories(target PUBLIC
    include
)
target_link_libraries(target PUBLIC 
    env
)
add_dependencies(target fbs_to_header)

target_sources(target PRIVATE
    src/target.cpp
    src/toolchain.cpp

    src/dynamic_target.cpp
    src/static_target.cpp

    src/fbs_loader.cpp
    src/fbs_storer.cpp

    src/util.cpp
)
target_include_directories(target PRIVATE
   ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(target PRIVATE 
    flatbuffers 
)

# Tests
if (BUILDCC_TESTING)
    set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()
