cmake_minimum_required(VERSION 3.20)
project(MsgDecrypter)


set(CMAKE_CXX_STANDARD 17)

find_package(sqlcipher CONFIG REQUIRED)
find_package(fmt REQUIRED)


add_executable(${PROJECT_NAME} MsgDecrypter.cpp
        SqliteUtils/SqliteBase.h
        Parser/Parser.cpp
        Parser/Parser.h
)

target_link_libraries(${PROJECT_NAME} sqlcipher::sqlcipher)
target_link_libraries(${PROJECT_NAME} fmt::fmt)