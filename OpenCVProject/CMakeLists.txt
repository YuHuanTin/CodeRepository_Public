
cmake_minimum_required(VERSION 3.20)
project(OpenCVProject)


find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)


set(CMAKE_CXX_STANDARD 17)

include_directories("D:\\Clion\\Winhttp_SimpleAPI")

add_executable(OpenCVProject TerrariaBlockFinder.cpp)

target_link_libraries(OpenCVProject "D:\\Clion\\Winhttp_SimpleAPI\\cmake-build-debug\\Winhttp_SimpleAPI.lib")
target_link_libraries(OpenCVProject fmt::fmt)
target_link_libraries(OpenCVProject nlohmann_json::nlohmann_json)
target_link_libraries(OpenCVProject opencv_ml opencv_dnn opencv_core opencv_flann opencv_highgui)