cmake_minimum_required(VERSION 3.15...4.1)
set(CMAKE_CXX_STANDARD 17)
add_library(window-app-utils)

find_package(Curses REQUIRED)

target_sources(window-app-utils
    PRIVATE WindowServer.cpp
    PRIVATE Window.cpp
    PRIVATE WTask.cpp
    PUBLIC  WindowServer.h
    PUBLIC  Window.h
    PUBLIC  WTask.h
    PUBLIC  TaskBase.h
)

target_include_directories(window-app-utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:h>
)

add_library(WindowServer::WindowServer ALIAS window-app-utils)

target_link_libraries(window-app-utils PRIVATE ${CURSES_LIBRARIES})
target_include_directories(window-app-utils PRIVATE ${CURSES_INCLUDE_DIRS})
target_include_directories(window-app-utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../json)
