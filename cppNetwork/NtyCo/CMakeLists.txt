cmake_minimum_required(VERSION 2.8)
project(MyLib)

# 设置输出路径为 ./lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 收集并编译 ./core 目录中的所有 .c 文件
file(GLOB CORE_SOURCES core/*.c)

foreach(SOURCE_FILE ${CORE_SOURCES})
    # 获取源文件的名称（不包含扩展名）
    get_filename_component(FILE_NAME ${SOURCE_FILE} NAME_WE)
    
    # 生成动态库
    add_library(${FILE_NAME}_shared SHARED ${SOURCE_FILE})
    
    # 生成静态库
    add_library(${FILE_NAME}_static STATIC ${SOURCE_FILE})

    # 设置动态库和静态库的输出名称为 FILE_NAME
    set_target_properties(${FILE_NAME}_shared PROPERTIES OUTPUT_NAME "${FILE_NAME}")
    set_target_properties(${FILE_NAME}_static PROPERTIES OUTPUT_NAME "${FILE_NAME}")
endforeach()

