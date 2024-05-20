add_library(dao "")

target_include_directories(dao
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/globals
    ${PROJECT_SOURCE_DIR}/src/dbcp_mysql
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(dao
    PRIVATE
    dao.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/dao.h
)

target_link_directories(dao
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(dao
    PUBLIC
    globals
    dbcp_mysql
)
