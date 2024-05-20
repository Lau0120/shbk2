add_executable(shbk2 main.cpp)

target_include_directories(shbk2
    PRIVATE ${SYSTEM_INCLUDE_DIRS}
)
target_link_directories(shbk2
    PRIVATE ${SYSTEM_LIBRARY_DIRS}
)

add_subdirectory(globals)
add_subdirectory(net_gate)
add_subdirectory(session)
add_subdirectory(msg_proxy)
add_subdirectory(events)
add_subdirectory(des)
add_subdirectory(log)
add_subdirectory(sys_config)
add_subdirectory(shbk2pb)
add_subdirectory(event_handlers)
add_subdirectory(dbcp_mysql)
add_subdirectory(dao)

target_link_libraries(shbk2
    PRIVATE
        globals
        net_gate
        session
        msg_proxy
        events
        des
        log
        sys_config
        shbk2pb
        event_handlers
        dbcp_mysql
        dao
    )
