# 
# HQL Base NetWork lib
#
cmake_minimum_required (VERSION 3.8)

project ("Hnw")

SET(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

set(CMAKE_CXX_STANDARD 11)

if(WIN32)
      message("WIN32 编译设置")
      ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS=1)
elseif(UNIX)
     message("UNIX 编译设置")
     set(CMAKE_C_COMPILER /usr/bin/gcc)
     set(CMAKE_CXX_COMPILER /usr/bin/g++)
     #通用的底层库 default lib
     set(DEFAULT_LIB pthread boost_system)
endif()

# 包含子项目。
#add_subdirectory ("./Hnw")
add_subdirectory ("./Hnw/Src")
add_subdirectory ("./Hnw/Example")

