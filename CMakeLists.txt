cmake_minimum_required(VERSION 3.27)
project(Utp_Game_Project_Logic)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_library(Utp_Game_Project_Logic SHARED main_GameState.cpp
        Game.cpp
        Game.h
        util.cpp
        util.h)

target_include_directories(Utp_Game_Project_Logic PRIVATE "/Library/Java/JavaVirtualMachines/adoptopenjdk-16.jdk/Contents/Home/include")
target_include_directories(Utp_Game_Project_Logic PRIVATE "/Library/Java/JavaVirtualMachines/adoptopenjdk-16.jdk/Contents/Home/include/darwin")