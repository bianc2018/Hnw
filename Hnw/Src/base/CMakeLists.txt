# CMakeList.txt: Hnw 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

#project ("HnwBase")
#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)
#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/OpenSSL.cmake)

add_library(HnwBase SHARED  "hnw_base.cpp" "hnw_base.h")
add_library(HnwBaseStatic   "hnw_base.cpp" "hnw_base.h")

SET_TARGET_PROPERTIES(HnwBase PROPERTIES VERSION ${HNW_VERSION})
SET_TARGET_PROPERTIES(HnwBaseStatic PROPERTIES VERSION ${HNW_VERSION})

if(OpenSSL_FOUND)
target_link_libraries(HnwBase PRIVATE ${DEFAULT_LIB} OpenSSL::SSL OpenSSL::Crypto Crypt32)
target_link_libraries(HnwBaseStatic PRIVATE ${DEFAULT_LIB} OpenSSL::SSL OpenSSL::Crypto Crypt32)
elseif()
target_link_libraries(HnwBase PRIVATE  ${DEFAULT_LIB})
target_link_libraries(HnwBaseStatic PRIVATE  ${DEFAULT_LIB})
endif()

install(TARGETS HnwBase HnwBaseStatic
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION bin
	ARCHIVE DESTINATION lib
)

install(FILES 
	${PROJECT_SOURCE_DIR}/Hnw/Src/base/hnw_base.h
	${PROJECT_SOURCE_DIR}/Hnw/Src/base/hnw_define.h
	DESTINATION include
)