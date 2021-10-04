set(FMT_INSTALL ON CACHE BOOL "Fmt install")
add_subdirectory(third_party/fmt)
# TODO, Remove fmt library generation and install target
# set_target_properties(fmt PROPERTIES EXCLUDE_FROM_ALL ON)
