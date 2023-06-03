cmake_minimum_required(VERSION 3.20)
project(TerrariaCVColorBlock)

find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(TerrariaCVColorBlock MinecraftAutoFisher.cpp)
target_link_libraries(TerrariaCVColorBlock nlohmann_json::nlohmann_json)
target_link_libraries(TerrariaCVColorBlock opencv_ml opencv_dnn opencv_core opencv_flann opencv_highgui)