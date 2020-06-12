# 
# HQL Base NetWork lib
#
cmake_minimum_required (VERSION 3.8)

#-DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static
#SET(CMAKE_TOOLCHAIN_FILE "D:\\lib\\vcpkg-master\\scripts\\buildsystems\\vcpkg.cmake")
#SET(VCPKG_TARGET_TRIPLET "x86-windows-static")
project ("Hnw")



SET(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

set(HNW_VERSION 1.1.0)
set(CMAKE_CXX_STANDARD 11)


if(WIN32)
      message("WIN32 编译设置")
      ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS=1)
elseif(UNIX)
     message("UNIX 编译设置")
     #通用的底层库 default lib
     set(DEFAULT_LIB pthread boost_system)
endif()

#依赖 boost openss
find_package(Boost 1.6.8 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

#<LibaryName>_FOUND
#<LibaryName>_INCLUDE_DIR or <LibaryName>_INCLUDES <LibaryName>_LIBRARY or <LibaryName>_LIBRARIES

#依赖 OpenSSL
find_package(OpenSSL 1.1.1)
if(OpenSSL_FOUND)
ADD_DEFINITIONS(-D_HNW_USE_OPENSSL=1)
include_directories(${OpenSSL_INCLUDE_DIR})
link_directories(${OpenSSL_LIBRARY_DIR})
else()
message(STATUS "OpenSSL not fonud")
endif()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 包含子项目。
#add_subdirectory ("./Hnw")
add_subdirectory ("./Hnw/Src/base")
add_subdirectory ("./Hnw/Src/http")
add_subdirectory ("./Hnw/Src/ssh")
if(BUILD_HNW_EXAMPLE)
add_subdirectory ("./Hnw/Example")
message(STATUS "BUILD_HNW_EXAMPLE ON ,build Example")
else()
message(STATUS "BUILD_HNW_EXAMPLE ON ,build Example")
endif()


#打包
# 构建一个 CPack 安装包
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE
  "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

  ###cpack
#-${vGitBranch}-${vGitCommit}-${vTimeStamp}
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${HNW_VERSION}")

include (CPack)