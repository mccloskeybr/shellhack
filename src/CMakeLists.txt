cmake_minimum_required(VERSION 3.5)
project(Shell_Hack)

include(GenerateExportHeader)

set(CMAKE_BUILD_TYPE Debug)
add_definitions(-DDEBUG)

include_directories(.)
add_subdirectory(common)
add_subdirectory(engine)
add_subdirectory(platform)
