add_library(des "")

target_include_directories(des
    PUBLIC
    ${PROJECT_SOURCE_DIR}/3rd/include
    ${PROJECT_SOURCE_DIR}/src/events
    ${PROJECT_SOURCE_DIR}/src/event_handlers
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(des
    PRIVATE
    des.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/des.h
)

target_link_directories(des
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(des
    PUBLIC
    events
    pthread
    event_handlers
)
