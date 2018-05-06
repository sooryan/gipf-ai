cmake_minimum_required(VERSION 3.9.6)
project(gipf)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fbuiltin -O0")
set(CMAKE_CXX_STANDARD 14)

include(UseSWIG)

find_package(Boost REQUIRED)
find_package(SWIG REQUIRED)
find_package(PythonLibs REQUIRED)

include_directories(include)
include_directories(gtsa/cpp/)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIRS})

# set(SOURCES src/mainapp.cpp src/Student.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gtsa/cpp/square.ttf
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SOURCES "src/*.cpp")

add_executable(simulator ${SOURCES})

set_property(SOURCE gipf.i PROPERTY CPLUSPLUS ON SWIG_MODULE_NAME gipf)
swig_add_library(gipf LANGUAGE python SOURCES gipf.i)
