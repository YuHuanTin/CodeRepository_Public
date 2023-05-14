cmake_minimum_required(VERSION 3.21)
project(Simulate)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS thread)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Simulate main.cpp)

target_link_libraries(Simulate ${Boost_LIBRARIES})
