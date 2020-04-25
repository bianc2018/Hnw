cmake_minimum_required(VERSION 2.8)



#linux
include_directories(/root/boost_1_68_0)
link_directories(/root/boost_1_68_0/stage/lib)	

#windows
include_directories($ENV{BOOST_1_68_0})
link_directories($ENV{BOOST_1_68_0}/lib)	
#set(Boost_INCLUDE_DIR $ENV{BOOST_1_68_0})
#set(Boost_LIBRARY_DIR $ENV{BOOST_1_68_0}//lib)

set(CMAKE_INCLUDE_PATH '$ENV{BOOST_1_68_0} ${CMAKE_INCLUDE_PATH}')
set(CMAKE_LIBRARY_PATH '$ENV{BOOST_1_68_0}//lib ${CMAKE_LIBRARY_PATH}')

set(CMAKE_INCLUDE_PATH '/root/boost_1_68_0 ${CMAKE_INCLUDE_PATH}')
set(CMAKE_LIBRARY_PATH '/root/boost_1_68_0/stage/lib ${CMAKE_LIBRARY_PATH}')

message(STATUS $ENV{BOOST_1_68_0})

#find
find_package(Boost 1.68.0)


if(Boost_FOUND)
message(STATUS "Boost is fonud:" ${Boost_INCLUDE_DIR} ",lib:" ${Boost_LIBRARY_DIR})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
elseif()
message(STATUS "Boost not fonud:" ${Boost_INCLUDE_DIR} ",lib:" ${Boost_LIBRARY_DIR})
endif()

