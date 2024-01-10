

find_package(Boost REQUIRED COMPONENTS thread)

link_libraries(Boost::boost Boost::thread)

add_executable(BoostThreadGroup main.cpp)

