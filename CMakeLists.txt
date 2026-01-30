cmake_minimum_required(VERSION 4.2)
set(CMAKE_CXX_STANDARD 20)
project(app CXX)

add_executable(app src/main.cpp)

add_subdirectory(src)
add_subdirectory(lib/window-app-utils)

target_link_libraries(app
    PRIVATE
        window-app-utils
        songbookapp
)