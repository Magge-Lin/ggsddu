cmake_minimum_required(VERSION 2.8)

Project(ZVNECT)

MESSAGE(STATUS "This is PROJECT_SOURCE Dir: " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "This is PROJECT_BINARY Dir: " ${PROJECT_BINARY_DIR})

add_subdirectory(src)

add_subdirectory(test)