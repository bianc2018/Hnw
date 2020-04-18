# CMakeList.txt: Hnw 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

#project ("HnwBase")
include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)

add_library(Hnw SHARED  "hnw_base.cpp" "hnw_base.h" "hnw_http.h" "hnw_http.cpp")

SET_TARGET_PROPERTIES(Hnw PROPERTIES VERSION 2.0)

install(TARGETS Hnw 
	LIBRARY DESTINATION Hnw/lib
	ARCHIVE DESTINATION Hnw/lib
)

install(FILES 
	${PROJECT_SOURCE_DIR}/Src/hnw_base.h
	${PROJECT_SOURCE_DIR}/Src/hnw_define.h
	${PROJECT_SOURCE_DIR}/Src/hnw_http.h
	DESTINATION Hnw/include
)