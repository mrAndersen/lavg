cmake_minimum_required(VERSION 3.10)
project(loadAvg)

find_package(CURL REQUIRED)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

if (${CURL_FOUND})
    message(STATUS "CURL FOUND ${CURL_INCLUDE_DIR} ${CURL_LIBRARIES}")
endif ()

set(CMAKE_CXX_STANDARD 14)
set(
        SOURCES
        avg.cpp

        vendor/fmt/src/posix.cc
        vendor/fmt/src/format.cc

        src/Http.cpp
        src/Http.h
)

add_executable(loadAvg ${SOURCES})
target_link_libraries(loadAvg ${CURL_LIBRARIES})
target_include_directories(loadAvg PUBLIC ${CURL_INCLUDE_DIR} vendor/fmt/include)
