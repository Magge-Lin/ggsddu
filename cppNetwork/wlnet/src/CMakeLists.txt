aux_source_directory(. SRC_DIR)

MESSAGE(STATUS "This is CMAKE_CURRENT_SOURCE Dir: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "SRC_DIR -> " ${SRC_DIR})

add_library(wlnet ${SRC_DIR})

