cmake_minimum_required(VERSION 3.10)
project(threadedSocketServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

find_library(PTHREAD pthread)
find_library(SQL sqlite3)
find_library(HELPER mqtthelper)

add_executable( tcpServer tcpServer.cpp dbAccess.cpp )
add_executable( testDb testDb.cpp dbAccess.cpp )

target_link_libraries( tcpServer ${SQL} ${PTHREAD} ${HELPER})
target_link_libraries( testDb ${SQL} ${PTHREAD} ${HELPER})
