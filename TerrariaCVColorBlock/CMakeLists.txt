cmake_minimum_required(VERSION 3.20)
project(TerrariaCVColorBlock)

find_package(OpenCV CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(TerrariaCVColorBlock main.cpp)

target_link_libraries(TerrariaCVColorBlock opencv_ml opencv_dnn opencv_core opencv_flann opencv_highgui)
