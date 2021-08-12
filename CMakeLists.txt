cmake_minimum_required(VERSION 3.10)
project(threadedSocketServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

find_library(PTHREAD pthread)
find_library(SQL sqlite3)

add_executable( tcpServer tcpServer.cpp )
target_link_libraries( tcpServer ${SQL} ${PTHREAD})
