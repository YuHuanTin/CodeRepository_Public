


set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

link_libraries(fmt::fmt)
link_libraries(nlohmann_json::nlohmann_json)
link_libraries(opencv_ml opencv_dnn opencv_core opencv_flann opencv_highgui)

add_executable(OpenCVProject TerrariaBlockFinder.cpp)
