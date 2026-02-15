cmake_minimum_required(VERSION 4.2)
set(CMAKE_CXX_STANDARD 20)
project(songbook CXX)

add_library(songbookapp)

find_package(SQLite3 REQUIRED)

target_sources(songbookapp
    PRIVATE SongBookApp.cpp
    PRIVATE SongDatabase.cpp
    PRIVATE SongBookUtils.cpp
    PUBLIC  SongBookApp.h
    PUBLIC  SongDatabase.h
    PUBLIC  SongBookUtils.h
)

target_include_directories(songbookapp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:h>
)

add_library(SongBookApp::SongBookApp ALIAS songbookapp)

target_link_libraries(songbookapp PRIVATE ${CURSES_LIBRARIES})
target_link_libraries(songbookapp PRIVATE curl)
target_include_directories(songbookapp PRIVATE ${CURSES_INCLUDE_DIRS})
target_include_directories(songbookapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/json)
target_include_directories(songbookapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tasks/h)

target_sources(songbookapp
    PUBLIC SongBookFormatter.h
    PRIVATE SongBookFormatter.cpp

    PUBLIC ../tasks/h/ListTask.h
    PUBLIC ../tasks/h/CRUDTask.h
    PUBLIC ../tasks/h/GatherTask.h
    PRIVATE ../tasks/cpp/ListTask.cpp
    PRIVATE ../tasks/cpp/CRUDTask.cpp
    PRIVATE ../tasks/cpp/GatherTask.cpp
)

target_link_libraries(songbookapp 
    PRIVATE 
        SQLite::SQLite3
        window-app-utils
)

target_include_directories(songbookapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/window-app-utils)