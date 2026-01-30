
cmake_minimum_required(VERSION 4.2)
project(sandbox CXX)

add_subdirectory(lib/window-app-utils)

add_executable(sandbox main2.cpp)
target_link_libraries(sandbox PRIVATE WindowServer::WindowServer)
