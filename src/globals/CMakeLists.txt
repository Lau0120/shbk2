add_library(globals "")

target_include_directories(globals
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/log
    ${PROJECT_SOURCE_DIR}/src/sys_config
)
target_sources(globals
    PRIVATE
    globals.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/globals.h
)

target_link_libraries(globals
    PUBLIC
    log
    sys_config
)
