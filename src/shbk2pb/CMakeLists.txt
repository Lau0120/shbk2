add_library(shbk2pb "")

target_include_directories(shbk2pb
    PRIVATE
    ${SYSTEM_INCLUDE_DIRS}
)
target_sources(shbk2pb
    PRIVATE
    shbk2.pb.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/shbk2.pb.h
)

target_link_directories(shbk2pb
    PRIVATE
    ${SYSTEM_LIBRARY_DIRS}
)
target_link_libraries(shbk2pb
    PUBLIC
    protobuf
    absl_log_internal_message
    absl_log_internal_check_op
)
