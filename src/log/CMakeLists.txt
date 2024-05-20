add_library(log "")

target_include_directories(log
    PUBLIC
    ${PROJECT_SOURCE_DIR}/3rd/include
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_sources(log
    PRIVATE
    log.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/log.h
)
