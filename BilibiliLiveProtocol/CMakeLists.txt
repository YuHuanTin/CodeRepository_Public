cmake_minimum_required(VERSION 3.21)

project(rundll)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

set(LIBRARY_FROM )
set(LIBRARY_FROM_WINHTTPAPI )

include_directories(${LIBRARY_FROM_WINHTTPAPI})
include_directories(${LIBRARY_FROM}/brotli/x86-Debug/include)
include_directories(${LIBRARY_FROM}/ixwebsocket/x86-Debug/include)

add_executable(websocket WebSocketTest.cpp)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(ZLIB_ROOT ${LIBRARY_FROM}/zlib/x86-Debug)
    set(ixwebsocket_DIR ${LIBRARY_FROM}/ixwebsocket/x86-Debug/lib/cmake/ixwebsocket)
    find_package(ZLIB REQUIRED)
    find_package(ixwebsocket CONFIG REQUIRED)
    target_link_libraries(websocket ${LIBRARY_FROM_WINHTTPAPI}/cmake-build-debug/Winhttp_SimpleAPI.lib)
    target_link_libraries(websocket ${LIBRARY_FROM}/brotli/x86-Debug/lib/brotlidec.lib)
    target_link_libraries(websocket ixwebsocket::ixwebsocket)
    target_link_libraries(websocket OpenSSL::SSL)
else()
    set(ZLIB_ROOT ${LIBRARY_FROM}/zlib/x86-Release)
    set(ixwebsocket_DIR ${LIBRARY_FROM}/ixwebsocket/x86-Release/lib/cmake/ixwebsocket)
    find_package(ZLIB REQUIRED)
    find_package(ixwebsocket CONFIG REQUIRED)
    target_link_libraries(websocket ${LIBRARY_FROM_WINHTTPAPI}/cmake-build-release/Winhttp_SimpleAPI.lib)
    target_link_libraries(websocket ${LIBRARY_FROM}/brotli/x86-Release/lib/brotlidec.lib)
    target_link_libraries(websocket ixwebsocket::ixwebsocket)
    target_link_libraries(websocket OpenSSL::SSL)
endif()
target_link_libraries(websocket ws2_32)
target_link_libraries(websocket crypt32)
target_link_libraries(websocket shlwapi)
