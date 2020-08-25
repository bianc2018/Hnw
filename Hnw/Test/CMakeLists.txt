#”√”⁄≤‚ ‘
cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/base)
include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/http)

#The package gtest is compatible with built-in CMake targets:
#    enable_testing()
#    find_package(GTest CONFIG REQUIRED)
#    target_link_libraries(main PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
#    add_test(AllTestsInMain main)

	enable_testing()
	find_package(GTest CONFIG REQUIRED)
if(GTest_FOUND)
	ADD_DEFINITIONS(-D_HNW_USE_GTest=1)
	include_directories(${GTest_INCLUDE_DIR})
	link_directories(${GTest_LIBRARY_DIR})

	add_executable(HnwTestHttpUtil "TestHttpUtil.cpp")
	target_link_libraries(HnwTestHttpUtil  PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main Boost::filesystem Boost::locale ${DEFAULT_LIB})
	add_test(HnwTest HnwTestHttpUtil)
elseif()
	message(STATUS "GTest not fonud")
endif()


