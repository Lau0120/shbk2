add_library(events "")

target_include_directories(events
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/globals
    ${PROJECT_SOURCE_DIR}/src/shbk2pb
    ${PROJECT_SOURCE_DIR}/src/session
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(events
    PRIVATE
    events.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/events.h
)

target_link_directories(events
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(events
    PUBLIC
    globals
    shbk2pb
    session
)
