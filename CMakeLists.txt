cmake_minimum_required(VERSION 3.17)
project(Passwords_Manager_GIT)

set(CMAKE_CXX_STANDARD 14)

add_executable(Passwords_Manager main.cpp parsing.cpp parsing.h clipboard.cpp clipboard.h encryption.cpp encryption.h)
