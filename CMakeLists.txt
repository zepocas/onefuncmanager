cmake_minimum_required(VERSION 3.27)
project(onefuncmanager)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

# Set the icon for the executable
set(ICON_FILE "icon.ico")

if (WIN32)
    set(WIN32_EXECUTABLE true)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows") # Add this line for windowed applications
endif()

add_executable(onefuncmanager
        onefuncmanager.cpp
        resources.rc)
