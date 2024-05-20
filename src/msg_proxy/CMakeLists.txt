add_library(msg_proxy "")

target_include_directories(msg_proxy
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/globals
    ${PROJECT_SOURCE_DIR}/src/session
    ${PROJECT_SOURCE_DIR}/src/events
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(msg_proxy
    PRIVATE
    msg_proxy.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/msg_proxy.h
)

target_link_directories(msg_proxy
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(msg_proxy
    PUBLIC
    globals
    session
    events
)
