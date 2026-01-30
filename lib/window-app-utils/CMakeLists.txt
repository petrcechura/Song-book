cmake_minimum_required(VERSION 3.15...4.1)

add_library(window-app-utils)

find_package(Curses REQUIRED)

target_sources(window-app-utils
    PRIVATE cpp/WindowServer.cpp
    PRIVATE cpp/Window.cpp
    PRIVATE cpp/WTask.cpp
    PUBLIC  h/WindowServer.h
    PUBLIC  h/Window.h
    PUBLIC  h/WTask.h
    PUBLIC  h/TaskBase.h
)

target_include_directories(window-app-utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/h>
        $<INSTALL_INTERFACE:h>
)

add_library(WindowServer::WindowServer ALIAS window-app-utils)

target_link_libraries(window-app-utils PRIVATE ${CURSES_LIBRARIES})
target_include_directories(window-app-utils PRIVATE ${CURSES_INCLUDE_DIRS})