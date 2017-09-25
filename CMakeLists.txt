project(vec_list-benchs)

cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W -Wall")
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pedantic")

add_subdirectory(benchs)
add_subdirectory(tests)
