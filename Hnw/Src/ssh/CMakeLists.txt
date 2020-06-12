#实现ssh 功能简化

# CMakeList.txt: Hnw 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
#依赖 LibSsh2
if(OpenSSL_FOUND)
	find_package(Libssh2)
if(Libssh2_FOUND)
	ADD_DEFINITIONS(-D_HNW_USE_LIBSSH2=1)
	include_directories(${Libssh2_INCLUDE_DIR})
	link_directories(${Libssh2_LIBRARY_DIR})
elseif()
	message(STATUS "Libssh2 not fonud")
endif()
endif()