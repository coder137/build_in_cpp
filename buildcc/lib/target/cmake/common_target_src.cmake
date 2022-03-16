set(COMMON_TARGET_SRCS 
    # Interfaces
    include/target/interface/builder_interface.h

    # Common
    src/common/target_config.cpp
    src/common/target_state.cpp
    include/target/common/target_config.h
    include/target/common/target_state.h
    include/target/common/target_env.h

    src/common/util.cpp
    include/target/common/util.h

    # API
    src/api/source_api.cpp
    src/api/include_api.cpp
    src/api/lib_api.cpp
    src/api/pch_api.cpp
    src/api/flag_api.cpp
    src/api/deps_api.cpp
    include/target/api/source_api.h
    include/target/api/include_api.h
    include/target/api/lib_api.h
    include/target/api/pch_api.h
    include/target/api/deps_api.h

    src/api/sync_api.cpp
    include/target/api/sync_api.h
    
    src/api/target_info_getter.cpp
    src/api/target_getter.cpp
    include/target/api/target_info_getter.h
    include/target/api/target_getter.h

    # Generator
    src/generator/generator.cpp
    include/target/generator.h

    # Target Info
    src/target_info/target_info.cpp
    include/target/target_info.h

    # Target friend
    src/target/friend/compile_pch.cpp
    src/target/friend/compile_object.cpp
    src/target/friend/link_target.cpp
    include/target/friend/compile_pch.h
    include/target/friend/compile_object.h
    include/target/friend/link_target.h

    # Target
    src/target/target.cpp
    src/target/build.cpp
    include/target/target.h
)
