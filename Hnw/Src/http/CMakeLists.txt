# CMakeList.txt: Hnw 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)

include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/base)

add_library(HnwHttp SHARED  "hnw_http.h" "hnw_http.cpp" )
add_library(HnwHttpStatic "hnw_http.h" "hnw_http.cpp" )

SET_TARGET_PROPERTIES(HnwHttp PROPERTIES VERSION ${HNW_VERSION})
SET_TARGET_PROPERTIES(HnwHttpStatic PROPERTIES VERSION ${HNW_VERSION})

target_link_libraries(HnwHttp  HnwBase ${DEFAULT_LIB} )
target_link_libraries(HnwHttpStatic  HnwBaseStatic ${DEFAULT_LIB} )

install(TARGETS HnwHttp  HnwHttpStatic
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION bin
	ARCHIVE DESTINATION lib
)

install(FILES 
	${PROJECT_SOURCE_DIR}/Hnw/Src/http/hnw_http.h
	DESTINATION include
)