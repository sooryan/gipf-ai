cmake_minimum_required(VERSION 3.9.6)
project(gipf)

include_directories(include)

# set(SOURCES src/mainapp.cpp src/Student.cpp)

file(GLOB SOURCES "src/*.cpp")

add_executable(simulator ${SOURCES})
