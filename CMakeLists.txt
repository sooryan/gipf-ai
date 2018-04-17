cmake_minimum_required(VERSION 3.9.6)
project(gipf)

find_package(Boost REQUIRED)

include_directories(include)
include_directories(gtsa/cpp/)
include_directories(${Boost_INCLUDE_DIR})

# set(SOURCES src/mainapp.cpp src/Student.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gtsa/cpp/square.ttf
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SOURCES "src/*.cpp")

add_executable(simulator ${SOURCES})
