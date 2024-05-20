add_library(session "")

target_include_directories(session
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/globals
    ${PROJECT_SOURCE_DIR}/src/events
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(session
    PRIVATE
    session.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/session.h
)

target_link_directories(session
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(session
    PUBLIC
    globals
    event
    events
)
