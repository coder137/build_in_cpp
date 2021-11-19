set(COMMON_TARGET_SRCS 
    # Interfaces
    include/target/interface/loader_interface.h
    include/target/interface/builder_interface.h

    # Common
    src/common/target_config.cpp
    src/common/target_state.cpp
    include/target/common/target_file_ext.h
    include/target/common/target_config.h
    include/target/common/target_state.h
    include/target/common/target_env.h
    include/target/common/target_type.h

    src/util/util.cpp
    include/target/common/util.h

    include/target/common/path.h

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
    include/target/api/flag_api.h
    include/target/api/deps_api.h

    src/api/copy_api.cpp
    include/target/api/copy_api.h
    
    src/api/target_info_getter.cpp
    src/api/target_getter.cpp
    include/target/api/target_info_getter.h
    include/target/api/target_getter.h

    # Base Generator
    src/generator/generator_loader.cpp
    src/generator/generator_storer.cpp
    include/target/base/generator_loader.h

    # Generator
    src/generator/generator.cpp
    include/target/generator.h

    # Target friend
    src/target/friend/compile_pch.cpp
    src/target/friend/compile_object.cpp
    src/target/friend/link_target.cpp
    include/target/friend/compile_pch.h
    include/target/friend/compile_object.h
    include/target/friend/link_target.h

    # Base Target
    src/target/target_loader.cpp
    src/target/target_storer.cpp
    include/target/base/target_loader.h
    include/target/base/target_storer.h

    # Target
    src/target/target.cpp
    src/target/build.cpp
    include/target/target.h
)
