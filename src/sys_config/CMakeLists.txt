add_library(sys_config "")

target_include_directories(sys_config
    PUBLIC
    ${PROJECT_SOURCE_DIR}/3rd/include
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_sources(sys_config
    PRIVATE
    sys_config.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/sys_config.h
)
