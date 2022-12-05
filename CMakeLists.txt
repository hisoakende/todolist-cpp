set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C++_COMPILER "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(todolist)

add_executable(todolist main.cpp)

add_library(db STATIC db/db.h db/db.cpp)

add_subdirectory(deps/libpqxx build-pqxx)

target_link_libraries(todolist db)

target_link_libraries(todolist pqxx)

target_link_libraries(db pqxx)
