cmake_minimum_required(VERSION 3.21)
project(rundll)

set(CMAKE_CXX_STANDARD 17)

include_directories(F:/brotli-1.0.9/c/include)
include_directories(F:/IXWebSocket-11.4.3)
include_directories(F:/zlib-1.2.13/cmake-build-debug)
include_directories(F:/zlib-1.2.13)
include_directories(D:/Clion/Winhttp_SimpleAPI)

add_executable(websocket main.cpp)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_link_libraries(websocket D:/.../Winhttp_SimpleAPI.lib)
    target_link_libraries(websocket D:/.../libssl32MDd.lib)
    target_link_libraries(websocket D:/.../libcrypto32MDd.lib)
    target_link_libraries(websocket F:/.../ixwebsocket.lib)
    target_link_libraries(websocket F:/.../zlibd.lib)
    target_link_libraries(websocket F:/.../brotlidec-static.lib)
    target_link_libraries(websocket F:/.../brotlicommon-static.lib)
else()
    
    
endif()
target_link_libraries(websocket ws2_32)
target_link_libraries(websocket crypt32)
target_link_libraries(websocket shlwapi)
