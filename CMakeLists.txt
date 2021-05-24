cmake_minimum_required(VERSION 3.16.3)

include_directories(${CMAKE_SOURCE_DIR} ../GUIslice/src/)
include_directories(${CMAKE_SOURCE_DIR} ../GUIslice/src/elem)
include_directories(${CMAKE_SOURCE_DIR} /usr/local/include)

link_directories(${CMAKE_SOURCE_DIR} /usr/local/lib)

project(heatingChamberTempCntrl)

add_executable(${PROJECT_NAME} main.c)