# CMakeList.txt: Hnw 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

#project ("HnwBase")
#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)
#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/OpenSSL.cmake)

if(BUILD_HNW_STATIC)
add_library(HnwBase  "hnw_base.cpp" "hnw_base.h")
message(STATUS "BUILD_HNW_STATIC ON ,build static HnwBase")
else()
add_library(HnwBase SHARED  "hnw_base.cpp" "hnw_base.h" )
message(STATUS "BUILD_HNW_STATIC OFF ,build shared HnwBase")
endif()

SET_TARGET_PROPERTIES(HnwBase PROPERTIES VERSION ${HNW_VERSION})

if(OpenSSL_FOUND)
target_link_libraries(HnwBase PRIVATE ${DEFAULT_LIB} OpenSSL::SSL OpenSSL::Crypto )
else()
target_link_libraries(HnwBase PRIVATE  ${DEFAULT_LIB})
endif()

install(TARGETS HnwBase
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION bin
	ARCHIVE DESTINATION lib
)

install(FILES 
	${PROJECT_SOURCE_DIR}/Hnw/Src/base/hnw_base.h
	${PROJECT_SOURCE_DIR}/Hnw/Src/base/hnw_define.h
	DESTINATION include
)