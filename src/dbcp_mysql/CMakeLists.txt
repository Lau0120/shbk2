add_library(dbcp_mysql "")

target_include_directories(dbcp_mysql
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(dbcp_mysql
    PRIVATE
    dbcp_mysql.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/dbcp_mysql.h
)

target_link_directories(dbcp_mysql
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(dbcp_mysql
    PUBLIC
    mysqlclient
)
