cmake_minimum_required(VERSION 2.8)

Project(WLNET)

SET(SRC_LIST test.cpp)

MESSAGE(STATUS "This is PROJECT_SOURCE Dir: " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "This is PROJECT_BINARY Dir: " ${PROJECT_BINARY_DIR})


include_directories(src)

add_subdirectory(src)

add_executable(helloworld ${SRC_LIST})

TARGET_LINK_LIBRARIES(helloworld wlnet)

install(TARGETS helloworld RUNTIME DESTINATION bin)

