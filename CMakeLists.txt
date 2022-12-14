set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C++_COMPILER "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(todolist)
add_executable(todolist main.cpp)

add_library(db STATIC db/db.h db/db.cpp)
add_library(utils STATIC utils/utils.h utils/utils.cpp)
add_library(validations STATIC validations/validations.h validations/validations.cpp)
add_library(users STATIC users/users.h users/users.cpp)
add_library(categories STATIC categories/categories.h categories/categories.cpp)
add_library(notes STATIC notes/notes.h notes/notes.cpp)

add_subdirectory(deps/libpqxx build-pqxx) 
add_subdirectory(deps/drogon)

find_package(OpenSSL REQUIRED)

target_link_libraries(todolist users)
target_link_libraries(todolist categories)
target_link_libraries(todolist notes)

target_link_libraries(todolist db)
target_link_libraries(todolist utils)
target_link_libraries(todolist validations)

target_link_libraries(todolist pqxx)
target_link_libraries(todolist drogon)

target_link_libraries(db pqxx)

target_link_libraries(utils db)

target_link_libraries(utils pqxx)
target_link_libraries(utils drogon)
target_link_libraries(utils OpenSSL::Crypto)

target_link_libraries(validations db)

target_link_libraries(validations drogon)
target_link_libraries(validations pqxx)

target_link_libraries(users utils)
target_link_libraries(users db)
target_link_libraries(users validations)

target_link_libraries(users drogon)
target_link_libraries(users pqxx)

target_link_libraries(categories utils)
target_link_libraries(categories db)
target_link_libraries(categories validations)

target_link_libraries(categories drogon)
target_link_libraries(categories pqxx)

target_link_libraries(notes utils)
target_link_libraries(notes db)
target_link_libraries(notes validations)

target_link_libraries(notes drogon)
target_link_libraries(notes pqxx)