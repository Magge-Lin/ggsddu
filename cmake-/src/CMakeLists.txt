# cmake_minimum_required(VERSION 2.8)
# PROJECT(SRC)

# SET(SRC_LIST main.c)
# MESSAGE(STATUS "This is PROJECT_SOURCE DIR: " ${PROJECT_SOURCE_DIR})
# MESSAGE(STATUS "This is PROJECT_BINARY DIR: " ${PROJECT_BINARY_DIR})

# #整体编译
# SET(LIB_SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/hello" "${CMAKE_CURRENT_SOURCE_DIR}/world")
# MESSAGE(STATUS "This is CMAKE_CURRENT_SOURCE DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

# foreach(LIB_SRC ${LIB_SRC_LIST})

#     include_directories(${LIB_SRC})

#     # add_subdirectory(${LIB_SRC})

#     aux_source_directory(${LIB_SRC} SRC_LIST)
#     MESSAGE(STATUS "LIB_SRC -> " ${LIB_SRC})
#     MESSAGE(STATUS "SRC_LIST -> " ${SRC_LIST})

# endforeach()

# add_executable(helloworld ${SRC_LIST})

# install(TARGETS helloworld RUNTIME DESTINATION bin)


cmake_minimum_required(VERSION 2.8)
PROJECT(SRC)

SET(SRC_LIST main.c)
MESSAGE(STATUS "This is PROJECT_SOURCE DIR: " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "This is PROJECT_BINARY DIR: " ${PROJECT_BINARY_DIR})

#整体编译
SET(LIB_SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/hello" "${CMAKE_CURRENT_SOURCE_DIR}/world")
MESSAGE(STATUS "This is CMAKE_CURRENT_SOURCE DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

foreach(LIB_SRC ${LIB_SRC_LIST})

    include_directories(${LIB_SRC})

    add_subdirectory(${LIB_SRC})

endforeach()

add_executable(helloworld ${SRC_LIST})

TARGET_LINK_LIBRARIES(helloworld hello world)

install(TARGETS helloworld RUNTIME DESTINATION bin)