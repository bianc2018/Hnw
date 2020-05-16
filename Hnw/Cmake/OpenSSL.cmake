#¼ì²éopenssl Ö§³Ö

cmake_minimum_required(VERSION 2.8)

message(STATUS "finding  OpenSSL")
#find
find_package(OpenSSL)

include_directories("C:\\Program Files\\OpenSSL-Win64\\include")
link_directories("C:\\Program Files\\OpenSSL-Win64\\lib")	

if(OpenSSL_FOUND)
message(STATUS "OpenSSL is fonud:" ${OPENSSL_INCLUDE_DIR} ",lib:" ${OPENSSL_LIBRARIES})
#set(CMAKE_INCLUDE_PATH '${OpenSSL_INCLUDE_DIR}  ${CMAKE_INCLUDE_PATH}')
#set(CMAKE_LIBRARY_PATH '${OpenSSL_LIBRARY_DIR}  ${CMAKE_INCLUDE_PATH}')
#include_directories(${OPENSSL_INCLUDE_DIR})
#link_directories(${OPENSSL_LIBRARIES})	
ADD_DEFINITIONS(-D_USE_OPENSSL)
elseif()
message(STATUS "OpenSSL not fonud:" ${OPENSSL_INCLUDE_DIR} ",lib:" ${OPENSSL_LIBRARIES})
endif()

