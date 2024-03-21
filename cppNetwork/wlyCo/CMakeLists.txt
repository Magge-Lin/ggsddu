cmake_minimum_required(VERSION 2.8)
project(WLYPROJECT)

# 设置输出路径为 ./lib 和 ./bin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 编译并生成动态库和静态库
file(GLOB SRC_SOURCES src/*.c)

foreach(SOURCE_FILE ${SRC_SOURCES})
    # 获取源文件的名称（不包含扩展名）
    get_filename_component(PROJECT_NAME ${SOURCE_FILE} NAME_WE)
    
    # 生成动态库
    add_library(${PROJECT_NAME}_shared SHARED ${SOURCE_FILE})
    
    # 生成静态库
    add_library(${PROJECT_NAME}_static STATIC ${SOURCE_FILE})

    # 设置动态库和静态库的输出路径和名称为 ./lib/lib{FILENAME}.so 和 ./lib/lib{FILENAME}.a
    set_target_properties(${PROJECT_NAME}_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
    set_target_properties(${PROJECT_NAME}_shared PROPERTIES LIBRARY_OUTPUT_NAME "${PROJECT_NAME}")
    
    set_target_properties(${PROJECT_NAME}_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
    set_target_properties(${PROJECT_NAME}_static PROPERTIES ARCHIVE_OUTPUT_NAME "${PROJECT_NAME}")
endforeach()

# 在 test 目录下编译生成可执行文件，并放置到 ./bin 目录下
add_subdirectory(test)

