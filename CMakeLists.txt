cmake_minimum_required(VERSION 3.16.3)

include_directories(${CMAKE_SOURCE_DIR} ../GUIslice/src/)
include_directories(${CMAKE_SOURCE_DIR} ../GUIslice/src/elem)
link_directories(${CMAKE_SOURCE_DIR} ../GUIslice/src/)
link_directories(${CMAKE_SOURCE_DIR} ../GUIslice/src/elem)

project(heatingChamberTempCntrl)

add_executable(${PROJECT_NAME} main.c)

