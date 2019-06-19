cmake_minimum_required(VERSION 3.10)
project(loadAvg)

set(CMAKE_CXX_STANDARD 14)
set(SOURCES avg.cpp)

add_executable(loadAvg ${SOURCES})
