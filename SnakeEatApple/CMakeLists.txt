

include_directories(easyx)

link_directories(easyx/x64)
link_libraries(EasyXa EasyXw)


add_executable(SnakeEatApple main.cpp)