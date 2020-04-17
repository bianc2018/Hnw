# 
# HQL Base NetWork lib
#
cmake_minimum_required (VERSION 3.8)

project ("Hnw")

SET(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS=1)

# 包含子项目。
#add_subdirectory ("./Hnw")
add_subdirectory ("./Hnw/Src")
add_subdirectory ("./Hnw/Example")

