# CMakeList.txt: Hnw 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

#project ("HnwBase")
include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)
include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/OpenSSL.cmake)

#add_library(HnwBase SHARED  "hnw_base.cpp" "hnw_base.h")
add_library(HnwBase   "hnw_base.cpp" "hnw_base.h")

SET_TARGET_PROPERTIES(HnwBase PROPERTIES VERSION ${HNW_VERSION})

target_link_libraries(HnwBase  ${DEFAULT_LIB} OpenSSL::SSL OpenSSL::Crypto)

install(TARGETS HnwBase 
	LIBRARY DESTINATION Hnw/lib
	ARCHIVE DESTINATION Hnw/lib
)

install(FILES 
	${PROJECT_SOURCE_DIR}/Src/base/hnw_base.h
	${PROJECT_SOURCE_DIR}/Src/base/hnw_define.h
	DESTINATION Hnw/include
)