cmake_minimum_required(VERSION 3.10...3.27)
  
project(shbk2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

set(SYSTEM_INCLUDE_DIRS
        /opt/homebrew/opt/mysql-client@8.0/include
        /opt/homebrew/opt/libevent/include
        /opt/homebrew/opt/protobuf/include
        /opt/homebrew/opt/abseil/include)
set(SYSTEM_LIBRARY_DIRS
        /opt/homebrew/opt/mysql-client@8.0/lib
        /opt/homebrew/opt/libevent/lib
        /opt/homebrew/opt/protobuf/lib
        /opt/homebrew/opt/abseil/lib)

add_compile_options(-Wno-deprecated)

add_subdirectory(src)
