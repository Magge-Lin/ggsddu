project(wlnet)
cmake_minimum_required(VERSION 2.8)

#设置库文件路径
#Set the library file path
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
MESSAGE(STATUS "LIBRARY_OUTPUT_PATH -> " ${LIBRARY_OUTPUT_PATH})
#设置可执行程序路径
#Set the executable program path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
MESSAGE(STATUS "EXECUTABLE_OUTPUT_PATH -> " ${EXECUTABLE_OUTPUT_PATH})
#设置子目录
#Set subdirectories
set(SUB_DIR_LIST "src")
#安装目录
#Installation directory
set(INSTALL_PATH_LIB lib)
set(INSTALL_PATH_INCLUDE include)

foreach(SUB_DIR ${SUB_DIR_LIST})
    #遍历源文件
    #Traverse source file
    aux_source_directory(${SUB_DIR} SRC_LIST)
    #安装头文件至系统目录
    #Install header file to system directory
    install(DIRECTORY ${SUB_DIR} DESTINATION ${INSTALL_PATH_INCLUDE} FILES_MATCHING PATTERN "*.h")
endforeach()

#使能c++11
#Enable c++11
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wno-predefined-identifier-outside-function)

#编译动态库
#Compile dynamic library
add_library(${PROJECT_NAME}_shared SHARED ${SRC_LIST})
target_include_directories(${PROJECT_NAME}_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}_shared ${LINK_LIB_LIST})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
install(TARGETS ${PROJECT_NAME}_shared  ARCHIVE DESTINATION ${INSTALL_PATH_LIB} LIBRARY DESTINATION ${INSTALL_PATH_LIB})

#编译静态库
#Compile static library
add_library(${PROJECT_NAME}_static STATIC ${SRC_LIST})
#引用头文件路径
#Reference header file path
target_include_directories(${PROJECT_NAME}_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
#安装静态库至系统目录
#Install static library to system directory
install(TARGETS ${PROJECT_NAME}_static ARCHIVE DESTINATION ${INSTALL_PATH_LIB})

add_subdirectory(test)
