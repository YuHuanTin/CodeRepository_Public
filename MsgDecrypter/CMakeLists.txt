cmake_minimum_required(VERSION 3.20)
project(MsgDecrypter)


set(CMAKE_CXX_STANDARD 17)


find_package(fmt REQUIRED)


add_executable(${PROJECT_NAME} MsgDecrypter.cpp)


target_link_libraries(${PROJECT_NAME} fmt::fmt)