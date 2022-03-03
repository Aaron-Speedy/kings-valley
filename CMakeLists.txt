cmake_minimum_required(VERSION 3.22)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main sfml-graphics)