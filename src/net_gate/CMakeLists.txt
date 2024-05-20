add_library(net_gate "")

target_include_directories(net_gate
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/globals
    ${PROJECT_SOURCE_DIR}/src/session
    ${PROJECT_SOURCE_DIR}/src/msg_proxy
    ${PROJECT_SOURCE_DIR}/src/des
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(net_gate
    PRIVATE
    net_gate.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/net_gate.h
)

target_link_directories(net_gate
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(net_gate
    PUBLIC
    globals
    event
    session
    msg_proxy
    des
)
