

set(CMAKE_CXX_STANDARD 17)

find_package(ixwebsocket CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-brotli CONFIG REQUIRED)

link_libraries(unofficial::brotli::brotlidec)
link_libraries(nlohmann_json::nlohmann_json)
link_libraries(cpr::cpr)
link_libraries(ixwebsocket::ixwebsocket)


add_executable(BilibiliLiveDanmuSpyer main.cpp)