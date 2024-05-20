add_library(event_handlers "")

target_include_directories(event_handlers
    PUBLIC
    ${PROJECT_SOURCE_DIR}/3rd/include
    ${PROJECT_SOURCE_DIR}/src/globals
    ${PROJECT_SOURCE_DIR}/src/events
    ${PROJECT_SOURCE_DIR}/src/dao
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(event_handlers
    PRIVATE
    event_handlers.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/event_handlers.h
)

target_link_directories(event_handlers
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(event_handlers
    PUBLIC
    globals
    events
    dao
    pthread
)
