cmake_minimum_required(VERSION 3.27)
project(onefuncmanager)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")


add_executable(onefuncmanager
        onefuncmanager.cpp)
