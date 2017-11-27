cmake_minimum_required(VERSION 2.6)
project(silluq)

add_executable(silluq main.cpp)

install(TARGETS silluq RUNTIME DESTINATION bin)
